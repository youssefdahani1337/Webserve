#include "../include/Client.hpp"
#include <dirent.h>

//return the status code

int	Client::DeleteDirectory(std::string dirPath)
{
	struct dirent	*EntryInfos;
	DIR				*dir;
	std::string		entryPath;
	struct stat		infos;
	int				status;
	std::string		entryName;

	dir = opendir(dirPath.c_str());
	if (dir == NULL)
		return (INTERNAL_SERVER_ERROR);
	while ((EntryInfos = readdir(dir)) != NULL)
	{
		entryName = EntryInfos->d_name;
		if (entryName == "." || entryName == "..")
			continue ;
		entryPath = dirPath + '/' + EntryInfos->d_name;
		if (stat(entryPath.c_str(), &infos) == -1)
			return (INTERNAL_SERVER_ERROR);
		if (!(infos.st_mode & S_IWUSR))
			return (FORBIDDEN);
		if (S_ISDIR(infos.st_mode))
		{
			status = this->DeleteDirectory(entryPath);
			if (status != NO_CONTENT)
				return (status);
			if (rmdir(entryPath.c_str()))
				return (INTERNAL_SERVER_ERROR);
		}
		else if (S_ISREG(infos.st_mode))
		{
			if (remove(entryPath.c_str()))
				return (INTERNAL_SERVER_ERROR);
		}
	}
	if (closedir(dir))
		return (INTERNAL_SERVER_ERROR);
	if (rmdir(dirPath.c_str()))
		return (INTERNAL_SERVER_ERROR);
	return (NO_CONTENT);
}

int Client::DeleteHandler()
{
    struct stat infos;
    std::string uri;
	int			status;

    if (stat(this->_path.c_str(), &infos))
		return (Tools::updateLogFile(NOT_FOUND, request->getMethod() , this->_server, "path to delete not found"), NOT_FOUND);
	if (!(infos.st_mode & S_IWUSR))
		return (Tools::updateLogFile(FORBIDDEN, request->getMethod() , this->_server, "don't have permissions to delete"), FORBIDDEN);
    if (S_ISREG(infos.st_mode))
	{
        if (remove(this->_path.c_str()))
			return (Tools::updateLogFile(INTERNAL_SERVER_ERROR, request->getMethod() , _server, "Error while removing file"), INTERNAL_SERVER_ERROR);
	}
    else if (infos.st_mode & S_IFDIR)
    {
		uri = this->request->getResource();
		if (uri.length() > 1 && uri.size() - 1 == '/')
			return (Tools::updateLogFile(CONFLICT, request->getMethod() , _server, "conflict in uri"), CONFLICT);
		status	= this->DeleteDirectory(_path);
		if (status != NO_CONTENT)
			return (Tools::updateLogFile(status, request->getMethod() , _server, "Error while deleting directory content"), status);
		if (rmdir(_path.c_str()))
			return (Tools::updateLogFile(INTERNAL_SERVER_ERROR, request->getMethod() , _server, "Error while removing directory"), INTERNAL_SERVER_ERROR);
    }
	return (NO_CONTENT);
}
