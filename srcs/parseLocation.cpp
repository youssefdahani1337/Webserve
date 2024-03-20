
#include "../include/Location.hpp"

void	Location::_copyCgiPass(std::map<std::string, std::string> cgiPassToCopy)
{
	std::map<std::string, std::string>::iterator	it;

	it = cgiPassToCopy.begin();
	while (it != cgiPassToCopy.end())
	{
		this->_cgiPass.insert(std::make_pair(it->first, it->second));
		it ++;
	}
	return ;
}

void	Location::_directiveNamesFcts(void)
{
	this->_directiveNames[0] = "location";
	this->_directiveNames[1] = "allow";
	this->_directiveNames[2] = "return";
	this->_directiveNames[3] = "root";
	this->_directiveNames[4] = "autoindex";
	this->_directiveNames[5] = "index";
	this->_directiveNames[6] = "cgi";
	this->_directiveNames[7] = "upload_path";
	this->_directiveFcts[0] = &Location::_ressourceDirective;
	this->_directiveFcts[1] = &Location::_allowedMethodsDirective;
	this->_directiveFcts[2] = &Location::_redirectionDirective;
	this->_directiveFcts[3] = &Location::_rootDirective;
	this->_directiveFcts[4] = &Location::_autoindexDirective;
	this->_directiveFcts[5] = &Location::_indexDirective;
	this->_directiveFcts[6] = &Location::_cgiDirective;
	this->_directiveFcts[7] = &Location::_uploadPathDirective;
	return ;
}

void	Location::_keyAndValue(std::string line, std::string &key, std::string &value)
{
	size_t				found;
	std::istringstream	issLine;
	std::istringstream	issKey;

	found = line.find(":");
	issLine.str(line);
	std::getline(issLine, key, ':');
	issKey.str(key);
	issKey >> key >> std::ws;
	if ((found == std::string::npos && (key.empty() || Configuration::keySpace(key) || key == "endLocation") && issKey.eof()))
	{
		issLine.clear();
		return ;
	}
	if (((found == std::string::npos || !issKey.eof())) || (found != std::string::npos && key.empty()))
		throw (std::runtime_error("ErrorL: unkown directives"));
	issKey.clear();
	std::getline(issLine, value);
	if (value.empty() || Configuration::keySpace(value))
		throw (std::runtime_error("ErrorL: found an empty value in directive"));
	issLine.clear();
	return ;
}

bool	Location::_isLocationDirective(std::string &key)
{
	int	i;

	i = -1;
	if (key == "cgi_pass")
		return (true);
	while (++i < 8)
	{
		if (this->_directiveNames[i] == key)
			return (true);
	}
	return (false);
}

void	Location::_ressourceDirective(std::string &value)
{
	std::istringstream	issValue(value);
	std::string			ressource;

	issValue >> ressource >> std::ws;
	if (!issValue.eof())
		throw (std::runtime_error("Error: invalid parameter in the location starting end point"));
	this->_ressource = ressource;
	return ;
}

void	Location::_allowedMethodsDirective(std::string &value)
{
	std::istringstream					issValue(value);
	std::string							method;
	std::vector<std::string>::iterator	it;
	bool								found;

	while (issValue >> method)
	{
		found = false;
		if (method != "GET" && method != "POST" && method != "DELETE")
		{
			std::cerr << "invalid method" << method;
			throw (std::runtime_error("in allow directive"));
		}
		it = this->_allowedMethods.begin();
		while (it != this->_allowedMethods.end())
		{
			if (*it == method)
			{
				found = true;
				break ;
			}
			it++;
		}
		if (!found)
			this->_allowedMethods.push_back(method);
	}
	return ;
}

void	Location::_redirectionDirective(std::string &value)
{
	std::istringstream	issValue(value);
	int					statusCode;
	std::string			path;

	issValue >> statusCode >> path >> std::ws;
	if (issValue.fail() || !issValue.eof())
		throw (std::runtime_error("Error: invalid parameter in the return directive"));
	if (statusCode < 0 || statusCode >= 1000)
	{
		std::cerr << "Error: invalid return code " << statusCode;
		throw (std::runtime_error(""));
	}
	if (path == "")
		throw (std::runtime_error("Error: empty path in the return directive"));
	this->_redirection = std::make_pair(statusCode, path);
	this->_isRedirectable = true;
	return ;
}

void	Location::_rootDirective(std::string &value)
{
	std::istringstream	issValue(value);
	std::string			root;

	issValue >> root >> std::ws;
	if (!issValue.eof())
		throw (std::runtime_error("Error: invalid parameter in the root directive"));
	this->_root = root;
	return ;
}

void	Location::_autoindexDirective(std::string &value)
{
	std::istringstream	issValue(value);
	std::string			autoIndex;

	issValue >> autoIndex >> std::ws;
	if (!issValue.eof() || (autoIndex != "on" && autoIndex != "off"))
		throw (std::runtime_error("Error: invalid parameter in the autoindex directive"));
	this->_autoIndex = (autoIndex == "on") ? true : false;
	return ;
}

void	Location::_indexDirective(std::string &value)
{
	std::istringstream	issValue(value);
	std::string			index;

	while (issValue >> index)
		this->_index.push_back(index);
	issValue >> std::ws;
	return ;
}

void	Location::_cgiDirective(std::string &value)
{
	std::istringstream	issValue(value);
	std::string			cgi;

	issValue >> cgi >> std::ws;
	if ((cgi != "on" && cgi != "off") || !issValue.eof())
		throw (std::runtime_error("Error: invalid parameter in the cgi directive"));
	this->_cgi = (cgi == "on") ? true : false;
	return ;
}

void	Location::_uploadPathDirective(std::string &value)
{
	std::istringstream	issValue(value);
	std::string			path;

	issValue >> path >> std::ws;
	if (!issValue.eof())
		throw (std::runtime_error("Error: invalid parameter in the upload_path directive"));
	this->_uploadPath = path;
	return ;
}

void	Location::_cgiPassDirective(std::vector<std::string> cgiInfos)
{
	std::istringstream					issValue;
	std::string							cgiPath;
	std::string							cgiExt;
	std::vector<std::string>::iterator	it;
	size_t								size;

	size = 0;
	it = cgiInfos.begin();
	while (it != cgiInfos.end())
	{
		issValue.str(*it);
		issValue >> cgiPath >> cgiExt >> std::ws;
		if (!issValue.eof())
			throw (std::runtime_error("Error: invalid parameter in cgi_pass directive"));
		this->_cgiPass.insert(std::make_pair(cgiPath, cgiExt));
		if (this->_cgiPass.size() == size)
			throw (std::runtime_error("Error: duplicate in cgi_pass directive"));
		size++;
		it ++;
		issValue.clear();
	}
	return ;
}

void	Location::_fillLocationDirectives(std::map<std::string, std::string> locationDirectives, std::vector<std::string> cgiInfos)
{
	std::map<std::string, std::string>::iterator	found;
	int												i;
	int												size;

	i = -1;
	size = 8;
	found = locationDirectives.find("allow");
	if (found == locationDirectives.end())
		this->_allowedMethods.push_back("GET");
	while (++i < size)
	{
		found = locationDirectives.find(this->_directiveNames[i]);
		if (found != locationDirectives.end())
		{
			(this->*(_directiveFcts[i]))(found->second);
			locationDirectives.erase(found);
		}
	}
	if (!cgiInfos.empty())
		this->_cgiPassDirective(cgiInfos);
	return ;
}

void	Location::_locationDirectives(std::istringstream &blockIss)
{
	std::string							key;
	std::string							value;
	std::string							line;
	std::map<std::string, std::string>	locationDirectives;
	size_t								size;
	std::vector<std::string>			cgiInfos;

	while (std::getline(blockIss, line))
	{
		value = "";
		this->_keyAndValue(line, key, value);
		if (key == "endLocation")
			break ;
		if (value.empty())
			continue ;
		if (!this->_isLocationDirective(key))
			throw (std::runtime_error("ErrorL: unkown directive"));
		if (key == "cgi_pass")
			cgiInfos.push_back(value);
		else
		{
			locationDirectives.insert(std::make_pair(key, value));
			if (locationDirectives.size() == size)
				throw (std::runtime_error("Error: found duplicate directive"));
			size = locationDirectives.size();
		}
	}
	if (size == 1)
		throw (std::runtime_error("Error: empty location"));
	this->_fillLocationDirectives(locationDirectives, cgiInfos);
	return ;
}
