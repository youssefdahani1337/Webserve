
#include "../include/Server.hpp"

void	Server::_checkDuplicateLocation(std::string ressource)
{
	std::vector<Location *>::iterator	it;
	std::vector<Location *>::iterator	ite;

	it = this->_locations.begin();
	ite = this->_locations.end();
	while (it != ite - 1)
	{
		if (ressource == (*it)->getRessource())
			throw (std::runtime_error("Error: found duplicate location"));
		it++;
	}
	return ;
}

void	Server::_maxBodySizeDirective(std::string &value)
{
	std::istringstream	issValue(value);
	size_t				maxSize;
	char				typeByte;

	issValue >> maxSize >> typeByte;
	if (!issValue.eof())
		issValue >> std::ws;
	if (issValue.fail() || !issValue.eof() || (typeByte != 'B' && typeByte != 'M' && typeByte != 'G'))
		throw (std::runtime_error("Error: invalid value in client_max_body_size directive"));
	if (typeByte == 'B')
		this->_maxBodySize = maxSize;
	else
		this->_maxBodySize = (typeByte == 'M') ? (maxSize * pow(1024, 2)) : (maxSize * pow(1024, 3));
	if (this->_maxBodySize > 10 * pow(1024, 3))
		throw (std::runtime_error("Error: invalid value in client_max_body_size directive"));
	return ;
}

void	Server::_copyErrPages(std::map<int, std::vector<std::string> > otherErrPages)
{
	std::map<int, std::vector<std::string> >::iterator	mapIt;
	std::map<int, std::vector<std::string> >::iterator	mapIte;

	mapIt = (otherErrPages).begin();
	mapIte = (otherErrPages).end();
	while (mapIt != mapIte)
	{
		this->_errorPages.insert(std::make_pair(mapIt->first, this->_copyVec<std::string>(mapIt->second)));
		mapIt++;
	}
	return ;
}

void	Server::_errorPagesDirective(std::vector<std::string> &errorPgs)
{
	std::istringstream	issValue;
	int					errorCode;
	std::string			errorPath;
	std::vector<std::string>::iterator	it, ite;
	std::map<int, std::vector<std::string> >::iterator	found;
	std::vector<std::string>	fstErrorPath;	

	it = errorPgs.begin();
	ite = errorPgs.end();
	while (it != ite)
	{
		issValue.str(*it);
		issValue >> errorCode >> errorPath >> std::ws;
		if (!issValue.eof())
			throw (std::runtime_error("Error: in the error_page directive"));
		if (errorCode < 300 || errorCode > 599)
			throw (std::runtime_error("Error: error code must be between 300 and 599 in error_page directive"));
		found = this->_errorPages.find(errorCode);
		if (found != this->_errorPages.end())
			(found->second).push_back(errorPath);
		else
		{
			fstErrorPath.push_back(errorPath);
			this->_errorPages.insert(std::make_pair(errorCode, fstErrorPath));
		}
		issValue.clear();
		fstErrorPath.clear();
		it ++;
	}
	return ;
}

void	Server::_serverNamesDirective(std::string &value)
{
	std::istringstream					issValue(value);
	std::string							serverName;
	std::vector<std::string>::iterator	it, ite;

	while (issValue >> serverName)
	{
		it = this->_serverNames.begin();
		ite = this->_serverNames.end();
		while (it != ite)
		{
			if (*it == serverName)
				throw (std::runtime_error("Error: found duplicate server names in a server"));
			it++;
		}
		this->_serverNames.push_back(serverName);
	}
	return ;
}

void	Server::_hostDirective(std::string &value)
{
	std::istringstream	issValue;
	std::string			host;
	int					hostByte;
	char				dot;
	int					byteNums;

	byteNums = 0;
	issValue.str(value);
	issValue >> host >> std::ws;
	if (!issValue.eof())
		throw (std::runtime_error("Error: invalid parameter in the host directive"));
	issValue.clear();
	issValue.str(host);
	while (issValue >> hostByte && byteNums < 4)
	{
		if (byteNums < 3)
			issValue >> dot;
		if ((byteNums < 3 && dot != '.') || hostByte < 0 || hostByte > 255)
			break ;
		byteNums ++;
	}
	if (dot != '.' || !issValue.eof())
	{
		std::cerr << "Error: host not found " << host ;
		throw (std::runtime_error(" in the hostdirective"));
	}
	this->_host = host;
	return ;
}

void	Server::_listen(std::string &value)
{
	std::istringstream	issValue;
	int					port;
	std::string			_port;

	issValue.str(value);
	issValue >> port >> std::ws;
	if (!issValue.eof())
		throw (std::runtime_error("Error: invalid parameter in the listen directive"));
	if (port <= 0 || port >= 65536)
	{
		std::cerr << "WebServer: invalid port " << port;
		throw (std::runtime_error(" of the listen directive"));
	}
	issValue.clear();
	issValue.str(value);
	issValue >> _port;
	this->_port = _port;
	return ;
}

void	Server::_fillServerDirectives(std::map<std::string, std::string> serverDirectives, std::vector<std::string> errorPages)
{
	std::map<std::string, std::string>::iterator	found;
	int	size;
	int	i;

	i = -1;
	size = 4;
	while (++i < 2)
	{
		found = serverDirectives.find(this->_directiveNames[i]);
		if (found == serverDirectives.end())
			throw (std::runtime_error("Error: necessary directive not found in the server block"));
		(this->*(_directiveFcts[i]))(found->second);
		serverDirectives.erase(found);
	}
	while (i < size)
	{
		found = serverDirectives.find(this->_directiveNames[i]);
		if (found != serverDirectives.end())
		{
			(this->*(_directiveFcts[i]))(found->second);
			serverDirectives.erase(found);
		}
		i++;
	}
	if (!errorPages.empty())
		this->_errorPagesDirective(errorPages);
	return ;
}

bool	Server::_isServerDirective(std::string &key)
{
	int	i;

	i = -1;
	if (key == "error_page")
		return (true);
	while (++i < 4)
	{
		if (this->_directiveNames[i] == key)
			return (true);
	}
	return (false);
}

void	Server::_keyAndValue(std::string line, std::string &key, std::string &value)
{
	size_t				found;
	std::istringstream	issLine;
	std::istringstream	issKey;

	found = line.find(":");
	issLine.str(line);
	std::getline(issLine, key, ':');
	issKey.str(key);
	issKey >> key >> std::ws;
	if ((found == std::string::npos && (key.empty() || Configuration::keySpace(key) || key == "endServer") && issKey.eof()))
	{
		issLine.clear();
		return ;
	}
	if ((found == std::string::npos || !issKey.eof()) || (found != std::string::npos && key.empty()))
		throw (std::runtime_error("Error: unkown directives"));
	issKey.clear();
	std::getline(issLine, value);
	if (value.empty() || Configuration::keySpace(value))
		throw (std::runtime_error("Error: found an empty value in directive"));
	issLine.clear();
	return ;
}

void	Server::_checkStartingPoint(std::string &line)
{
	size_t								found;
	std::istringstream					issLine;
	std::string							key;
	std::istringstream					issKey;
	std::string							value;

	found = line.find(":");
	issLine.str(line);
	std::getline(issLine, key, ':');
	issKey.str(key);
	issKey >> key >> std::ws;
	if (key == "server" && found != std::string::npos && issKey.eof())
	{
		std::getline(issLine, value);
		if (!value.empty() && !Configuration::keySpace(value))
			throw (std::runtime_error("Error: in the server block starting end point"));
		issKey.clear();
		issLine.clear();
	}
	else
		throw (std::runtime_error("Error: in the server block starting end point"));
	return ;
}

void	Server::_serverDirectives(std::istringstream &blockIss)
{
	std::string							key;
	std::string							value;
	std::string							line;
	std::map<std::string, std::string>	serverDirectives;
	std::vector<std::string>			errorPages;
	size_t								size;

	std::getline(blockIss, line);
	this->_checkStartingPoint(line);
	while (std::getline(blockIss, line))
	{
		value = "";
		this->_keyAndValue(line, key, value);
		if (key == "endServer")
			break ;
		if (value.empty() || ((key.empty() || Configuration::keySpace(key)) && value.empty()))
			continue ;
		if (!this->_isServerDirective(key))
			throw (std::runtime_error("Error unkown directive"));
		if (key == "error_page")
			errorPages.push_back(value);
		else
		{
			serverDirectives.insert(std::make_pair(key, value));
			if (serverDirectives.size() == size)
				throw (std::runtime_error("Error: found duplicate directive"));
			size = serverDirectives.size();
		}
	}
	if (serverDirectives.empty())
		throw (std::runtime_error("Error: necessary directives messing in server"));
	this->_fillServerDirectives(serverDirectives, errorPages);
	return ;
}

void	Server::_directiveNamesFcts(void)
{
	this->_directiveNames[0] = "listen";
	this->_directiveNames[1] = "host";
	this->_directiveNames[2] = "server_name";
	this->_directiveNames[3] = "client_max_body_size";
	this->_directiveFcts[0] = &Server::_listen;
	this->_directiveFcts[1] = &Server::_hostDirective;
	this->_directiveFcts[2] = &Server::_serverNamesDirective;
	this->_directiveFcts[3] = &Server::_maxBodySizeDirective;
	return ;
}
