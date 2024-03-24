 
#include "../include/Configuration.hpp"

void	Configuration::deleteServVec(std::vector<Server *> &servVec)
{
	size_t	size;

	size = servVec.size();
	for (size_t i = 0; i < size; i++)
		delete(servVec[i]);
	return ;
}

void	Configuration::duplicateServersNames(std::vector<std::string> serverNames1, std::vector<std::string> serverNames2, std::vector<Server *> &sameHostPort)
{
	std::vector<std::string>::iterator	iti;
	std::vector<std::string>::iterator	itj;
	std::vector<std::string>::iterator	itie;
	std::vector<std::string>::iterator	itje;

	iti = serverNames1.begin();
	itie = serverNames1.end();
	itje = serverNames2.end();
	while (iti != itie)
	{
		itj = serverNames2.begin();
		while (itj != itje)
		{
			if (*iti == *itj)
			{
				Configuration::deleteServVec(sameHostPort);
				throw (std::runtime_error("Error: found duplicate server Names conflict"));
			}
			itj++;
		}
		iti++;
	}
	return ;
}

void	Configuration::getServersSameHostPort(std::vector<Server *> &servers, std::map<int, std::vector<Server *> >	&serversMap)
{
	std::vector<Server *>::iterator			iti;
	std::vector<Server *>::iterator			itj;
	std::vector<Server *>					sameHostPort;
	int										key;

	key = 0;
	iti = servers.begin();
	while (iti != servers.end())
	{
		sameHostPort.push_back(*iti);
		servers.erase(iti);
		itj = iti;
		while (itj != servers.end())
		{
			if ((*sameHostPort.begin())->getIp() == (*itj)->getIp() && (*sameHostPort.begin())->getPort() == (*itj)->getPort())
			{
				Configuration::duplicateServersNames((*sameHostPort.begin())->getServerNames(), (*itj)->getServerNames(), sameHostPort);
				sameHostPort.push_back(*itj);
				servers.erase(itj);
				itj--;
			}
			itj++;
		}
		serversMap.insert(std::make_pair(--key, sameHostPort));
		sameHostPort.clear();
	}
	return ;
}

bool	Configuration::keySpace(std::string &key)
{
	size_t	i;

	i = 0;
	while (isspace(key[i]))
		i++;
	key = &key[i];
	return (key.empty() ? true : false);
}

void	Configuration::checkRemaining(std::string  &configFile)
{
	std::istringstream		configFileIss;
	std::string				line;

	configFileIss.str(configFile);
	while (std::getline(configFileIss, line))
	{
		Configuration::keySpace(line);
		if (!line.empty())
			throw (std::runtime_error("Error: unknown directive"));
	}
	return ;
}

Server	*Configuration::serverBlockParsing(std::string &serverBlock)
{
	std::string					locationString;
	std::vector<std::string >	locationsBlocks;
	Server						*server = NULL;

	while (1)
	{
		locationString = Configuration::blockString(serverBlock, "location", "endLocation");
		if (locationString.empty())
			break ;
		locationsBlocks.push_back(locationString);
	}
	if (!locationsBlocks.size())
		throw (std::runtime_error("Error: Locations missing in a server block"));
	server = new Server(serverBlock, locationsBlocks);
	return (server);
}

std::string	Configuration::blockString(std::string &source, char const *begStr, char const *endStr)
{
	size_t		endBlockPoints[2];
	std::string	blockStr;
	std::string	endingStr(endStr);

	endBlockPoints[0] = source.find(begStr);
	if (endBlockPoints[0] == std::string::npos)
		return (blockStr);
	endBlockPoints[1] = source.find(endStr, endBlockPoints[0]);
	if (endBlockPoints[1] == std::string::npos)
		throw (std::runtime_error("Error: ending block string not found"));
	blockStr = source.substr(endBlockPoints[0], endBlockPoints[1] + endingStr.length() - endBlockPoints[0]);
	source.erase(source.begin() + endBlockPoints[0], source.begin() + endBlockPoints[1] + endingStr.length());
	return (blockStr);
}

void	Configuration::eraseComments(std::string &configFile)
{
	size_t	commentFound;
	size_t	endComment;

	commentFound = 0;
	while (1)
	{
		commentFound = configFile.find("#", commentFound);
		if (commentFound == std::string::npos)
			break ;
		endComment = configFile.find("\n", commentFound);
		if (endComment == std::string::npos)
		{
			configFile.erase(configFile.begin() + commentFound,configFile.end());
			return ;
		}
		configFile.erase(configFile.begin() + commentFound, configFile.begin() + endComment + 1);
	}
	return ;
}

void	Configuration::serversCreate(std::string &configFile, std::vector<Server *> &servers)
{
	std::string	serverString;

	while (1)
	{
		serverString = Configuration::blockString(configFile, "server", "endServer");
		if (serverString.empty())
			break ;
		if (servers.size() == MAX_SERVERS)
			throw (std::runtime_error("Error: exceeded the maximum numbers of servers."));
		servers.push_back(Configuration::serverBlockParsing(serverString));
	}
	if (servers.empty())
		throw (std::runtime_error("Error: no server found"));
	return ;
}

std::map<int, std::vector<Server *> >	Configuration::parseConfigFile(std::string &configFileName)
{
	std::string				configFileString;
	std::stringstream		Buffer;
	std::vector<Server *>	servers;
	std::ifstream			configFile;
	std::map<int, std::vector<Server *> >	serversMap;

	configFile.open(configFileName.c_str());
	if (!configFile.is_open())
		throw (std::runtime_error("Error: could not open configuration file"));
	Buffer << configFile.rdbuf();
	configFileString = Buffer.str();
	Configuration::eraseComments(configFileString);
	try
	{
		Configuration::serversCreate(configFileString, servers);
		Configuration::checkRemaining(configFileString);
		Configuration::getServersSameHostPort(servers, serversMap);
	}
	catch(const std::exception& e)
	{
		Configuration::deleteServVec(servers);
		std::map<int, std::vector<Server *> >::iterator	itm, itmend;
		itm = serversMap.begin();
		itmend = serversMap.end();
		while (itm != itmend)
		{
			Configuration::deleteServVec(itm->second);
			itm++;
		}
		throw ;
	}
	return (serversMap);
}
