
#include "../include/Request.hpp"

/* Start Ortodox Canonical Form */
Request::Request() :endRequest (false), headerCount(0), fileCreated(false), chunked(false),gotChunkSize(false), contentLength(0), byte_count(0)
{
	srand(time(NULL));
}
Request::Request(const Request& re):Message(re) { *this = re;}

Request& Request::operator=(const Request& re)
{
	if (this != &re)
	{
		this->_method = re._method;
		this->_resource = re._resource;
		endRequest = false;
		headerCount = 0;
	}
	return (*this);
}

Request::~Request() {}

/*End*/

/* Start GEtters and setters*/

std::string			Request::getResource(void)  { return (this->_resource); }
const std::string&	Request::getMethod(void)  const  { return (this->_method); }
long				Request::getContentLength(void) { return (byte_count); }
const std::string&	Request::getLogDetails(void)  const  { return (logDetails); }
const std::string&	Request::getFileName(void)  const  { return (this -> fileName); }
bool				Request::getEndRequest() { return (endRequest); }
bool				Request::getFileCreated() { return (fileCreated); }
std::string 		Request::getQuery() const {return (_query);}
std::string 		Request::getUri() const {return (_uri);}
void				Request::setResource(std::string resource) { this->_resource = resource;}
void				Request::setMethod(std::string method) { this->_method = method; }
void				Request::setEndRequest() { endRequest = true; }
void				Request::setFile() { file.close(); }
void				Request::setLogDetails(std::string str) { logDetails = str; }

/* End */

/*Parsing the request line */

bool Request::parseUri(int & code)
{
	std::istringstream inputStr(_resource);
	std::string 	str;

	if (getline(inputStr, str, '#') && !inputStr.eof())
	{
		_fragment = inputStr.str().substr(inputStr.tellg());
		if (str.empty())
		{
			logDetails = "empty Res [parse fragment]";
			code = BAD_REQUEST;
			return (false);
		}
		_resource = str;
	}
	inputStr.clear();
	inputStr.str(_resource);
	if (getline(inputStr, str, '?') && !inputStr.eof())
	{
		_query = inputStr.str().substr(inputStr.tellg());
		if (str.empty())
		{
			logDetails = "empty Res [parse query]";
			code = BAD_REQUEST;
			return (false);
		}
		_resource = str;
	}

	char c;
	
	for (size_t i = 0; i < _resource.length(); i++)
	{
		c = _resource[i];
		if (strchr(INVALID_CHARS, c) )
		{
			logDetails = "Invalid chars in uri";
			code  = BAD_REQUEST;
			return (false);
		}
	}
	return (true);
}

bool	Request::uri_decode()
{
	std::ostringstream iss;
	char 				str[3] = {0};
	char 				c;
	int 				v;	
	for (size_t i = 0; i < _resource.length(); i++)
	{
		c = _resource.at(i); 
		if (c == '%')
		{
			if (i + 2  <_resource.length() && 
				std::isxdigit(_resource[i + 1]) && std::isxdigit(_resource[i + 2]))
			{
				str[0] = _resource[i+1];
				str[1] = _resource[i+2];
				v = strtol (str,NULL,16);
				iss << static_cast<char>(v);
				i += 2;
			}
			else
			{
				logDetails = "Uri decode";
				return (false);
			}
		}
		else
			iss << c;
	}
	_resource = iss.str();
	return (true);
}
int Request::parseRequestLine(std::string & buffer, ParseReq_St & requestLine)
{
	std::istringstream inputStr(buffer);
	std::string			str;
	int					code;

	if (!std::getline(inputStr, str) || inputStr.eof())
	{
		if (endRequest)
		{
			logDetails = "Request Line incomplete";
			return (BAD_REQUEST);
		}
		if (inputStr.str().length() > 1039)
		{
			logDetails = "request line Too long";
			return (URI_TOO_LONG);
		}
		return (1);
	}

	buffer = inputStr.str().substr(inputStr.tellg());

	if (!str.empty() && str[str.size() -1] == '\r')
		str.resize(str.size() - 1);
	if (str.empty())
	{
		logDetails = "Request line empty";
		return (BAD_REQUEST);
	}

	inputStr.str(str);
	str.clear();

	inputStr >> _method >> _resource >> str ;
	if (!inputStr.eof() || inputStr.fail())
		return (logDetails = "fail in url", BAD_REQUEST);
	if (_method != "GET" && _method != "POST" && _method != "DELETE")
			return (logDetails="Method not exist",METHOD_NOT_ALLOWED);
	if (_resource.empty())
		return (logDetails="empty Res",BAD_REQUEST);
	if(_resource.length() > 1023)
		return (logDetails="URI Too long",URI_TOO_LONG);
	_uri = _resource;
	if (!parseUri(code))
		return (code);
	if (!uri_decode())
		return (BAD_REQUEST);
	if (str != _version)
	{
		logDetails = "HTTP version not supported";
		return (HTTP_VERSION_NOT_SUPPORTED);
	}

	requestLine = ReadReqL;
	return (0);
}

/*Parsing the headers fileds*/
int Request::parseHeader(std::string & buffer, ParseReq_St &readHeader)
{
	std::istringstream inputStream(buffer);
	std::string str;
	std::string key, value;
	short i;

	while (std::getline(inputStream, str) && !inputStream.eof())
	{
		i = 1;
		if (str.empty() || (str == "\r"))
		{
			buffer = buffer.substr(str.length() + i);
			readHeader = ReadHeaders;
			return (0);
		}
		if (str[str.size() -1] == '\r' && ++i)
			str.resize(str.size() - 1);
		if (str.length() >= MAX_BUFFER_HEADER)
			return (URI_TOO_LONG);
		
		buffer = buffer.substr(str.length() + i);
		std::istringstream line(str);
		if (!std::getline(line, key, ':'))
			return (BAD_REQUEST);
		line >> std::ws;
		if (!std::getline(line, value))
			return (BAD_REQUEST);
		if (!line.eof() || line.fail())
			return (BAD_REQUEST);
		if (value.empty())
			return (BAD_REQUEST);
		if (!addHeader(Tools::toUpper(key), value))
			return (BAD_REQUEST);
		if (++headerCount > MAX_HEADERS)
			return (REQUEST_HEADER_FIELDS_TOO_LARGE);
	}
	if (buffer.length() >= MAX_BUFFER_HEADER)
		return (BAD_REQUEST);
	return (1);
}

/*Debuging functions */

void Request::printHeaderH()
{
	std::map<std::string , std::string >::iterator it;

	it = _headers.begin();
	while (it != _headers.end())
	{
		std::cout << it->first << ":" << it->second << "\n";
		it ++;
	}
}
void Request::showRequest()
{
	std::cout << "Request line\n"; 
	std::cout << "------------------------\n";
	std::cout << this->_method << "|" << this->_resource << "|" 
				<< this->_query <<  "|"
				<< this->_version << "\n";

	std::cout << "------------------------\n";
	std::cout << "Headers\n"; 
	std::cout << "------------------------\n";
	
	this->printHeaderH();

	std::cout << "------------------------\n";
	std::cout << "BODY\n"; 
	std::cout << "------------------------\n";
	std::cout << _body ;
	std::cout << "------------------------\n";

}