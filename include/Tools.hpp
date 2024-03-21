#pragma once

#include "Macros.hpp"
#include <sys/stat.h>
#include "Server.hpp"
#include <cstdlib>
#include <iomanip>

typedef		std::map<std::string, std::string> mapStrStr; 

class Server;

class Tools
{
private:
	Tools();
	Tools& Tool(Tools & );

public:
	static long						identity;
	static mapStrStr                _types;
	static std::ofstream			logFile;
	
	static std::string  intTOstr(int );
	static std::string  getBodyBegin(std::string );
	static std::string  getBodyEnd(void);
	static std::string  messageCode(int );
	static std::string  getExtension(const std::string &);
	static time_t       getTime(void);
	static std::string makeAnchor(const char *, const char *);


	static void         setTypes(void);
	static std::string  findExtension(std::string );
	static std::string  getMimeType(const std::string & );
	static bool			checkIfPathValid(std::string & );
	static void         updateLogFile(int statusCode, std::string method, Server *server, std::string details);
	static std::string  getCurrentTime();
	static std::string	findFileName(const std::string & );
	static std::string	RandomFile();
	static void			sighandler(int);
	static std::string	toUpper(std::string );
	static std::string 	realPath(std::string);
};