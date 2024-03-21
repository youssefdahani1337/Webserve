#include "../include/Client.hpp"

bool    Client::checkCGI()
{
    if (response->getStatus() == READING_FILE)
    {    
        if (_location && _location->getCGI() && !(_cgiPath= _location->isCGIFile(response->getFile())).empty())
            return (true); 
        if (isPost())
        {
            request->setLogDetails("the file is not supported in cgi");
            _statusCode = FORBIDDEN;
            buildErrorPage();
            return (false);
        }
    } 
    return (false);
}
void    Client::buildErrorPage()
{
    response->setFile(_server->getFileError(_statusCode));
    if (response->getFile().empty() || !response->checkReading())
    {
        response->generatePage(_statusCode);//STREAM
        return ;
    }
    response->setStatus(READING_FILE);
}

void    Client::handleRedir()
{
    std::string location ;
    
    location.assign("http://");
    location += request->getHeaderValue("host");
    location += _location->getRedirPath();
    _statusCode = MOVED_PERMANENTLY;
    
    response->handleRedir(_statusCode , location);
}

void    Client::handleAutoIndex()
{
    if (!response->listDir(request->getResource(), _path))
    {
        response->setStatus(ERROR);
        _statusCode = INTERNAL_SERVER_ERROR;
        request->setLogDetails("listing dir");
        return ;
    }
    _statusCode = SUCCESS;
}

void    Client::handleDirectory()
{
    response->setFile(_location->getBestIndex(_path));
    if (!response->getFile().empty())
    {
        if (!response->checkReading())
        {
            request->setLogDetails("Reading permission index file");
            _statusCode = FORBIDDEN;
        }
        else
            response->setStatus(READING_FILE);
    }
    else if (_location->getAutoIndex())
        handleAutoIndex();
    else
    {
        request->setLogDetails("No index , no auto_index");
        response->setStatus(ERROR);
        _statusCode = FORBIDDEN;
    }
}

void       Client::checkResource()
{
    struct stat st;
    std::string res;
    
    _statusCode = SUCCESS;

    if (stat(_path.c_str(), &st) == 0)
    {
        if (S_ISDIR(st.st_mode))
        {
            res = request->getResource();
            if (res.length() > 1 && res.at(res.size() - 1) != '/')
            {
                res.append("/");
                response->setStatus(REDIR);
                _location->setRedir(301, res);
            }
            response->setStatus(DIRECTORY);
            return ;
        }
        if (!(st.st_mode & S_IRUSR))
        {
            _statusCode = FORBIDDEN;
            request->setLogDetails("Right of reading file or dir in Res func");
            response->setStatus(ERROR);
            return ;
        }
        if (S_ISREG(st.st_mode))
        {
            response->setStatus(READING_FILE);
            response->setFile(_path);
            response->setFileSize(st.st_size);
            return ;
        }
      
    }
    request->setLogDetails("Resource not found in root path");
    _statusCode = NOT_FOUND;
    response->setStatus(ERROR);
}


bool      Client::handleResponse()
{
    response->setStatus(begin_RES);
    if (_server == NULL)
        _server =*_servers->begin();
        
    if (_statusCode >= 400 && _statusCode <= 599)
        response->setStatus(ERROR);
    else if (_statusCode >= 200 && _statusCode <= 299 && !_location->getCGI())
            response->generatePage(_statusCode);
    else if (_location && _location->isRedir())
        response->setStatus(REDIR);
    else
        checkResource();    

    if (response->getStatus() == REDIR)
       handleRedir();
    if (response->getStatus() == DIRECTORY)
        handleDirectory();
    if (response->getStatus() == ERROR)
        buildErrorPage();
    if (checkCGI())
        runCgi();
    return (false);
}

