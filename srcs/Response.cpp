#include "../include/Response.hpp"

Response::Response() 
{
	fd = -1;
	_headerCgi  = false;
	_lenHeader = 0;
}
Response::~Response()
{
	closeFd();
}

Response::Response(const Response& re):Message(re) { *this = re; }

Response& Response::operator=(const Response& re)
{
	if (this != &re)
	{
		Response::Message::operator= (re);
		this->_statusCode = re._statusCode;
	}
	return (*this);
}

std::string			Response::getFile() const  { return (this->_fileName); }
int					Response::getStatus() const { return (this->status); }
const std::string&	Response::getLogDetails(void)  const  { return (logDetails); }

void				Response::setFileSize(size_t size) { _fileSize = size; }
void				Response::setFile(std::string file) { this->_fileName = file; }
void				Response::setFd(int fd){this->fd = fd;}
void				Response::setStatus(enum Status e) { this->status = e; }
void				Response::setLogDetails(std::string str) { logDetails = str; }
void				Response::closeFd() { if (fd != -1) close(fd);}

void				Response::setheaderCgi(bool val, size_t lenHeader)
{
	_headerCgi = val;
	_lenHeader = lenHeader;
}

bool    Response::checkReading()
{
	struct stat st;
	if (stat(_fileName.c_str(), &st) == 0)
	{
		if (S_ISREG(st.st_mode)	&& (st.st_mode & S_IRUSR))
			return (1);
		status = ERROR;
		_statusCode = FORBIDDEN;
	}
	return (0);
}

void	Response::handleRedir(int statusCode, std::string location)
{
	_statusCode = statusCode;
	
	if ((_statusCode >= 300 && _statusCode<= 307) || _statusCode == 307 || _statusCode == 308)
	{
		addHeader("Location", location);
		_body.clear();
	}
	else
	{
		addHeader("Content-Type", "text/plain");
		addHeader("Content-Length", Tools::intTOstr(location.length()));
	}
	status = STREAM_FILE;
	_body.assign(location);
}

bool	Response::listDir(std::string uri, std::string path)
{
	DIR* dir  = opendir(path.c_str());
	struct dirent 	*entry;
	std::string 		str;

	if (!dir)
		return (false);

	addHeader("Content-Type", 		"text/html");
	addHeader("Transfer-Encoding",  "chunked");	

	_body = Tools::getBodyBegin("Index of /");
	_body +="<h1> Index of "+ uri+" </h1>";
	_body += "<hr> <pre>";
	while ((entry = readdir(dir)))
	{
		if (entry->d_name[0] == '.' && strcmp(entry->d_name , "..") != 0)
			continue;
		str.assign(entry->d_name);
		if (entry->d_type == DT_DIR)
			str.append("/");
		_body += Tools::makeAnchor(str.c_str(), str.c_str());
	}
	_body += "</pre>";
	_body += Tools::getBodyEnd();
	status = STREAM;
	closedir(dir);
	return (true);
}

void	Response::generatePage(int statusCode)
{
	_statusCode = statusCode;
	_body = Tools::getBodyBegin(Tools::messageCode(_statusCode));
	_body += "<h1>" + Tools::intTOstr(_statusCode);
	_body += " " + Tools::messageCode(_statusCode) + "</h1>";
	_body += Tools::getBodyEnd();

	addHeader("Content-Length", Tools::intTOstr(_body.length()));
	addHeader("Content-Type", "text/html");
	status = STREAM_FILE;
}

bool	Response::openFile()
{
	fd = open(_fileName.c_str(), O_RDONLY);
	if (fd == -1)
	{
		status = STREAM_FILE;
		logDetails = "Can't open file in generating response";
		generatePage(FORBIDDEN);
		return (false);
	}
	if (status == READING_FILE)
		addHeader("Content-Type", Tools::getMimeType(_fileName));
	else
		addHeader("Content-Type", "text/html");
	addHeader("Transfer-Encoding", "Chunked");
	return(true) ;
}

void	Response::generateRequestline()
{
	std::stringstream iss;
	messageContent.clear();
	iss <<  _version << " " << _statusCode << " " << Tools::messageCode(_statusCode) << "\r\n";
	messageContent = iss.str();
}

void	Response::generateHeaders()
{
	std::map<std::string , std::string>::iterator it, itend;
	std::stringstream iss;

	it = _headers.begin();
	itend = _headers.end();

	addHeader("Server", "WebWaiters");
	while (it != itend)
	{
		iss << it->first << ": " << it->second << "\r\n";
		it ++;
	}
	messageContent += iss.str();
	if (!_headerCgi)
		messageContent += "\r\n";
	else
	{
		char *buff = new char[_lenHeader + 1];
		read(fd,buff, _lenHeader);
		messageContent += std::string (buff, _lenHeader);
		delete[] buff;
	}
}

void	Response::generateResponse(short int statusCode)
{
	messageContent.clear();
	_statusCode = statusCode;
	if (_statusCode == 0)
		_statusCode = SUCCESS;

	generateRequestline();
	generateHeaders();

	if (status == STREAM)
		makeChunked();
	else if (status == STREAM_FILE)
		messageContent += _body;
}

void	Response::makeChunked()
{
	unsigned int 	len = 0;
	std::string 	value;

	while (!_body.empty())
	{
		if (_body.length() < 1023)
		{
			value = _body;
			len = _body.length();
			_body.clear();
		}
		else
		{
			value =  _body.substr(0, 1023);
			len = 1023;
			_body = _body.substr(1023, _body.length());
		}
		std::stringstream iss;
		iss << std::hex << std::uppercase <<  len;
		messageContent += iss.str() + "\r\n";
		messageContent += value + "\r\n";
	}

	messageContent += "0\r\n\r\n";
}


std::string Response::getChunk(bool & endResponse)
{
	ssize_t 				len;
	std::string 			str;
	std::stringstream 		iss;

	if (!messageContent.empty())
	{

		if (messageContent.length() >= 1023)
		{
			str = messageContent.substr(0, 1023);
			messageContent = messageContent.substr(1023);
		}
		else
		{
			str = messageContent;
			messageContent.clear();
			if ((status == STREAM || status == STREAM_FILE))
			endResponse = false;
		}
		return (str);
	}

	char *buffer = new char[BUFFER_SIZE + 8];
	len = read(fd, buffer, BUFFER_SIZE - 1);
	if (len <= 0)
	{
		delete[] buffer;
		endResponse = false;
		return ("0\r\n\r\n\0");
	}
	buffer[len] = 0;
	
	iss << std::hex << std::uppercase <<  len;
	str.append(iss.str());
	str.append("\r\n");
	str.append(buffer, len);
	str.append("\r\n");
	delete []buffer;
	return (str);	
}

//debug


void Response::printresponse()
{
	std::cout << "==================================\n";
	std::cout << messageContent;
	std::cout << "==================================\n";
}