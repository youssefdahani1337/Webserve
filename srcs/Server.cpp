#include "../include/Server.hpp"

Server::Server(std::string &serverBlock, std::vector<std::string> &locations)
{
	std::vector<std::string>::iterator	it, ite;
	std::istringstream					issBlock;
	Location							*createLocation;

	this->_directiveNamesFcts();
	issBlock.str(serverBlock);
	this->_serverDirectives(issBlock);
	issBlock.clear();
	it = locations.begin();
	ite = locations.end();
	try
	{
		while (it != ite)
		{
			createLocation = new Location(*it);
			(this->_locations).push_back(createLocation);
			this->_checkDuplicateLocation(createLocation->getRessource());
			it++;
		}
	}
	catch (std::exception const &e)
	{
		for (size_t i = 0; i < this->_locations.size(); i++)
			delete (this->_locations[i]);
		throw ;
	}
	if (this->_locations.empty())
		throw (std::runtime_error("Error: Locations missing in a server block"));
	return ;
}

Server::~Server()
{
	for (size_t i = 0; i < _locations.size(); i++)
		delete _locations[i];
	return ;
}

Server::Server(Server const &_)
{
	*this = _;
	return ;
}

template <typename T>
std::vector<T>	Server::_copyVec(std::vector<T> src)
{
	typename std::vector<T>::iterator	it, ite;
	std::vector<T>						copy;

	it = src.begin();
	ite = src.end();
	while (it != ite)
	{
		copy.push_back(*it);
		it ++;
	}
	return (copy);
}

Server& Server::operator=(Server const &other)
{
	std::vector<std::string>::iterator					vecStrIt;
	std::vector<Location *>::iterator					vecLocationIt;
	std::vector<std::string>							errPaths;

	if (this == &other)
		return (*this);
	this->_port = other._port;
	this->_host = other._host;
	this->_serverNames = this->_copyVec<std::string>(other._serverNames);
	this->_copyErrPages(other._errorPages);
	this->_maxBodySize = other._maxBodySize;
	this->_locations = this->_copyVec<Location *>(other._locations);
	return (*this);
}

void Server::setServerFd(int ServerFd)
{
    this -> _serverFd = ServerFd;
	return ;
}

std::string Server::getPort() const { return (this -> _port);}

int Server::getServerFd() const { return (this -> _serverFd); }

std::string Server::getIp() const { return (this -> _host); }

std::vector<std::string>	Server::getServerNames() const { return (this->_serverNames); }
void	Server::sortLocation()
{
	size_t size = _locations.size();
	Location *l;
	for (size_t i = 0; i < size; i++)
	{
		for (size_t j = i + 1; j < size; j++)
		{
			if (_locations[i]->getRessource().length() < _locations[j]->getRessource().length())
			{
				l = _locations[i];
				_locations[i] = _locations[j];
				_locations[j] = l;
			}
		}
		
	}
	
}


Location* Server::getMatchLocation(std::string url, std::string & path)
{
	sortLocation();
	size_t size = _locations.size();
	std::string res, rootPath;

	for (size_t i = 0; i < size; i++)
	{
		res = _locations[i]->getRessource(); 
		if (url.find(res) == 0)
		{
			rootPath = _locations[i]->getRootPath();
			
			if (*rootPath.rbegin() == '/' && rootPath != "/")
				rootPath.resize(rootPath.size() -1);
			path = rootPath + url;
			return (new Location(*_locations[i]));
		}
	}
	return (NULL);
}

std::string Server::getFileError(int errorCode)
{
	std::map<int, std::vector<std::string> >::iterator it;
	it = _errorPages.find(errorCode);
	return (it != _errorPages.end() ? *(it->second.begin()): "");
}

long long  Server::getMaxBodySize() const { return (_maxBodySize);}
