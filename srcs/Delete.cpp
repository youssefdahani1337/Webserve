#include "../include/Client.hpp"

void	Client::dirContent(DIR *dir, std::string dirPath, int &status)
{
	std::string		entryPath;
	struct dirent	*EntryInfos;
	struct stat		infos;
	int				newStatus;
	bool			statusAssigned;

	statusAssigned = false;
	newStatus = NO_CONTENT;
	while ((EntryInfos = readdir(dir)) != NULL)
	{
		if (!strcmp(EntryInfos->d_name, ".") || !strcmp(EntryInfos->d_name, ".."))
			continue ;
		entryPath = dirPath + '/' + EntryInfos->d_name;
		if (stat(entryPath.c_str(), &infos) == -1)
			newStatus = INTERNAL_SERVER_ERROR;
		else
		{
			if (!(infos.st_mode & S_IWUSR) || (S_ISDIR(infos.st_mode) & !(infos.st_mode & S_IRUSR)))
				newStatus = FORBIDDEN;
			else if (S_ISDIR(infos.st_mode))
				newStatus = this->DeleteDirectory(entryPath);
			else if (S_ISREG(infos.st_mode))
			{
				if (remove(entryPath.c_str()))
					newStatus = INTERNAL_SERVER_ERROR;
			}
		}
		if (statusAssigned == false && newStatus != NO_CONTENT)
		{
			status = newStatus;
			statusAssigned = true;
		}
	}
	return ;
}

int	Client::DeleteDirectory(std::string dirPath)
{
	DIR				*dir;
	int				status;

	dir = opendir(dirPath.c_str());
	if (dir == NULL)
		return (INTERNAL_SERVER_ERROR);
	status = NO_CONTENT;
	this->dirContent(dir, dirPath, status);
	if (closedir(dir) || rmdir(dirPath.c_str()))
	{
		if (status == NO_CONTENT)
			return (INTERNAL_SERVER_ERROR);
	}
	return (status);
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
    if (infos.st_mode & S_IFREG)
	{
        if (remove(this->_path.c_str()))
			return (Tools::updateLogFile(INTERNAL_SERVER_ERROR, request->getMethod() , _server, "Error while removing file"), INTERNAL_SERVER_ERROR);
	}
    else if (infos.st_mode & S_IFDIR)
    {
		uri = this->request->getResource();
		if (*(uri.end() - 1) != '/')
			return (Tools::updateLogFile(CONFLICT, request->getMethod() , _server, "conflict in uri"), CONFLICT);
		if (!(infos.st_mode & S_IRUSR))
			return (Tools::updateLogFile(FORBIDDEN, request->getMethod() , this->_server, "don't have permissions to delete directory"), FORBIDDEN);
		status	= this->DeleteDirectory(_path);
		if (status != NO_CONTENT)
			return (Tools::updateLogFile(status, request->getMethod() , _server, "Error while deleting directory content"), status);
    }
	return (NO_CONTENT);
}
