#include "../include/Tools.hpp"

mapStrStr Tools::_types;
long	Tools::identity = 0;
std::string Tools::intTOstr(int num)
{
    std::ostringstream iss;
    iss << num;
    return (iss.str());
}

std::string Tools::getBodyBegin(std::string title)
{
	std::string str;
	str = 	"<!DOCTYPE html>"
			"<html lang='en'>"
			"<head>"
			"<meta charset='UTF-8'>"
			"<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
			"<title>"+title+"</title>"
			"<style>h1{text-align:center;}</style>"
			"</head>"
			"<body>";
		return (str);
}

std::string Tools::getBodyEnd()
{
	return ("<hr></body></html>");
}



std::string Tools::getExtension(const std::string & fileName)
{
	size_t	pos = fileName.find_last_of('.');
	
	if (pos == std::string::npos)
		return ("");
	return (fileName.substr(pos+1));
}

time_t	Tools::getTime()
{
	return (std::time(0));
}

std::string Tools::makeAnchor(const char *str)
{
	std::string anchor;

	anchor.append("<a href='");
	anchor.append(str);
	anchor.append("'>");
	anchor.append(str);
	anchor.append("</a>\n");
	return (anchor);
}

void Tools::setTypes()
{
	std::ifstream file;
    std::string str;
	
	file.open("data/mime.types", std::ios::in);
	
    int first;
    int end;
    if (!file.is_open())
		throw std::runtime_error("Error:Mime type file ");
	while (std::getline(file, str))
	{
		first = str.find_first_of(" ");
		end = str.find_last_of(" ");
		_types[str.substr(end + 1)] = str.substr(0, first);
	}
    file.close();
}

std::string  Tools::findExtension(std::string contentType)
{
   if (contentType.find("multipart/form-data") != std::string::npos)
       throw BAD_REQUEST;

	mapStrStr::iterator it, itend;

	it = _types.begin();
	itend = _types.end();

	while (it != itend)
	{
		if (it->second == contentType)
			return ("." + it->first);
		it ++;	
	}
	throw BAD_REQUEST;
    return ("");
}

std::string Tools::getMimeType(const std::string & filename)
{
	mapStrStr::iterator it;
	std::string 		extension;

	extension = getExtension(filename);
	if (extension.empty())
		return ("text/plain");
	it = _types.find(extension);
	return (it != _types.end() ? it->second : "text/plain");
}

bool Tools::checkIfPathValid(std::string &path)
{
    struct stat infos;

    if (!stat(path.c_str(), &infos))
	{
        if (!access(path.c_str(), W_OK) && S_ISDIR(infos.st_mode))
			return (true);
	}
	else
	{
		std::string cmd = "mkdir -m 777 -p ";
		cmd += path;
		return(system(cmd.c_str()), true);	
	}
    return (false);
}
std::string Tools::getCurrentTime()
{
	std::stringstream str;
	std::time_t currentTime = getTime();
	std::tm *local = std::localtime(&currentTime);

	str << std::setw(4) << std::setfill('0') << (local -> tm_year + 1900) << "-" <<
		std::setw(2) << std::setfill('0') << (local -> tm_mon + 1) << "-"
			<< std::setw(2) << std::setfill ('0') << (local -> tm_mday) << " "
				<< std::setw(2) << std::setfill('0') << (local -> tm_hour) << ":"
					<< std::setw(2) << std::setfill('0') << (local -> tm_min) << ":"
						<< std::setw(2) << std::setfill('0') << (local -> tm_sec);

	return (str.str());
}
void  Tools::updateLogFile(int statusCode,std::string method,  Server *server, std::string details)
{
	std::string time = getCurrentTime();

	logFile << time ;
	if (server)
	{
	 	logFile << " Server<" + server-> getIp() + "::" + server -> getPort();
	}
	else
		logFile << " Unknown Server  " ;
	if (!method.empty())
		logFile << " " << method ;
	else
		logFile << "Unknown Method ";
		
	logFile << " OUTPUT <" << statusCode << ": " + details + ">" << std::endl;
}

std::string	Tools::findFileName(const std::string & path)
{
	size_t pos;

	pos = path.rfind('/');

	return (path.substr(pos +1));
}

std::string Tools::RandomFile()
{
	std::string filename = "./tmp/Webserv/";
	
	if (!checkIfPathValid(filename))
		return ("");
	filename.append("Webserv_");
	filename.append(intTOstr(getTime()));
	return (filename);
}

void	Tools::sighandler(int num)
{
	(void)num;
}
std::string	Tools::toUpper(std::string str)
{
	for (size_t i = 0; i < str.length(); i++)
		str[i] = std::tolower(str[i]);
	return(str);
}

std::string Tools::realPath(std::string path)
{
	char real[PATH_MAX];
	if (!realpath(path.c_str(), real))
		return ("");
	return (std::string(real));
}