
#include "../include/Location.hpp"


Location::Location(std::string &locationBlock) : _isRedirectable(false), _autoIndex(false), _cgi(false)
{
	std::istringstream	issBlock;

	this->_directiveNamesFcts();
	issBlock.str(locationBlock);
	this->_locationDirectives(issBlock);
	issBlock.clear();
	return ;
}

Location::~Location()
{
	return ;
}

Location::Location(Location const &_)
{
	*this = _;
	return ;
}

Location& Location::operator=(Location const &other)
{
	std::vector<std::string>::iterator	vecIt;
	std::vector<std::string>			otherAllowedMethods;

	if (this == &other)
		return (*this);
	this->_ressource = other._ressource;
	otherAllowedMethods = other._allowedMethods;
	vecIt = otherAllowedMethods.begin();
	while (vecIt != otherAllowedMethods.end())
	{
		this->_allowedMethods.push_back(*vecIt);
		vecIt++;
	}
	this->_redirection = std::make_pair(other._redirection.first, std::string(other._redirection.second));
	this->_isRedirectable = other._isRedirectable;
	this->_root = other._root;
	this->_autoIndex = other._autoIndex;
	this->_index = other._index;
	this->_cgi = other._cgi;
	this->_copyCgiPass(other._cgiPass);
	this->_uploadPath = other._uploadPath;
	return (*this);
}
std::string Location::getRessource() const { return(_ressource); }

bool Location::isAllowed(const std::string & method)
{
	std::vector<std::string>::iterator it;
	it = std::find(_allowedMethods.begin(), _allowedMethods.end(), method);
	if (it == _allowedMethods.end())
		return (false);
	return (true);
}
bool 	Location::isRedir() const 
{
	return (_isRedirectable);
}
std::string Location::getRedirPath() const  
{
	return (_redirection.second);
}
int Location::getRedirCode() const 
{
	return (_redirection.first);
}

void Location::setRedir(int code, std::string str)
{
	_isRedirectable = true;
	_redirection.first = code;
	_redirection.second = str;
}
bool Location::getCGI()
{
	return (_cgi);
}
std::string Location::isCGIFile(const std::string &  fileName)
{
	std::string ex = "." + Tools::getExtension(fileName);
	mapStrStr::iterator it, itend;
	it = _cgiPass.begin();
	itend = _cgiPass.end();
	std::cout << ex << "<<<<\n";
	while (it != itend)
	{
		if (ex == it->second)
			return (it->first);
		it ++;
	}
	return ("");
}

std::string Location::getBestIndex(std::string p)
{
	std::vector<std::string>::iterator it;
	it = _index.begin();
	while (it != _index.end())
	{
		std::string path = p + "/" + *it;
		if (access(path.c_str(), F_OK) == 0)
			return (path);
		it ++;
	}
	return ("");
}
bool Location::getAutoIndex()
{
	return(_autoIndex);
}
std::string Location::getRootPath()
{
	return (_root);
}
std::string Location::getUploadPath() const { return(_uploadPath); }
