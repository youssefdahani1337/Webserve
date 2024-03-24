#pragma once

# include <iostream>
# include <fstream>
# include <sstream>
# include "Server.hpp"

class Server;

class Configuration
{
    private:
		static std::string				blockString(std::string &source, char const *begStr, char const *endStr);
		static Server					*serverBlockParsing(std::string &serverBlock);
		static void						eraseComments(std::string &configFile);
		static void						checkRemaining(std::string &configFile);
		static void						duplicateServersNames(std::vector<std::string> serverNames1, std::vector<std::string> serverNames2, std::vector<Server *> &sameHostPort);
		static void						serversCreate(std::string &configFile, std::vector<Server *> &servers);
		static void						getServersSameHostPort(std::vector<Server *> &servers, std::map<int, std::vector<Server *> > &serversMap);
	public:
		static bool						keySpace(std::string &key);
		static void						deleteServVec(std::vector<Server *> &servVec);
		static std::map<int, std::vector<Server *> >	parseConfigFile(std::string &configFileName);
};