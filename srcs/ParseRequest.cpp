#include "../include/Client.hpp"

void    Client::readBuffer(void)
{
    char bufRead[BUFFER_SIZE] = {0};
    ssize_t readVal;
    readVal = read(_fdClient, &bufRead, BUFFER_SIZE - 1);
    if (readVal == -1)
    {
        Tools::updateLogFile(0,request->getMethod(), _server, "can't read from the client");
        throw std::runtime_error("Error");
    }
    bufRead[readVal] = 0;
    buffer += std::string(bufRead, readVal);
}

bool Client::parseRequest()
{
    readBuffer();
    if (status == begin)
    {
        if ((_statusCode = request->parseRequestLine(buffer, status)))
            return (this->_statusCode == 1 ? 1 : 0);
    }
    if (status == ReadReqL)
    {
        if ((_statusCode = request->parseHeader(buffer, status)))
        {   
            if (status != 1)
                request->setLogDetails("parsing Headers");
            return (this->_statusCode == 1 ? 1 : 0);
        }
        if (!getResPath() || !isMethodAllowed())
        {
            request->setLogDetails("Check path or method not allowed");
            return (0);
        }
    }
    if (status == ReadHeaders)
    {
        if (isPost())
        {
            request->addBody(buffer);
            this->buffer.clear();
            _statusCode = PostHandler();
            if (_statusCode)
                return (this->_statusCode == NOT_YET ? NOT_YET :FINISH);
        }
        if (isDelete())
        {
            _statusCode = DeleteHandler();
            return (FINISH);
        }
        return (FINISH); //Get
    }
    return(1);
}

bool    Client::checkPath()
{
    char realPathLocation[PATH_MAX];
    char realPathResource[PATH_MAX];

    if (!realpath(_location->getRootPath().c_str(), realPathLocation) ||
        !realpath(_path.c_str(), realPathResource))
    {
        _statusCode = NOT_FOUND;
        return (false);
    }
    if (strncmp(realPathLocation, realPathResource, strlen(realPathLocation)) != 0)
    {
        _statusCode = BAD_REQUEST;
        return (false); 
    }
    _path.assign(realPathResource);
    return(true);
}
bool Client::getResPath()
{    
    std::string host = request->getHeaderValue("host");
    if (host.empty() || !whichServer())
    {
        response->setStatus(ERROR);
        _statusCode = BAD_REQUEST;
        return (false);
    }
    if ((_location = _server->getMatchLocation(request->getResource(), _path)) == NULL)
    {
        _statusCode = NOT_FOUND;
        return (false);
    }
    if (!checkPath())
        return (false);
    return (true);
}

std::string Client::parseHost(std::string host)
{
    size_t pos;
    if (host.find("http://") == 0)
        host.erase(0, 8);
    if ((pos = host.find_last_of(':')) != std::string::npos)
        host.erase(pos, std::string::npos);
    return (host);
}

bool    Client::whichServer()
{
    std::vector<Server * >::iterator it , itend;
    std::vector<std::string> serverTmp;
    std::vector<std::string>::iterator itStr;
    std::string host;

    it = _servers->begin(); 
    itend = _servers->end();
    host = parseHost(request->getHeaderValue("host"));
    while (it != itend)
    {

        serverTmp = (*it)->getServerNames();
        itStr = std::find(serverTmp.begin(), serverTmp.end(), host);
        if (itStr != serverTmp.end())
        {
            _server = (*it);
            return(true) ;
        }
        it ++;
    }
    _server = *(_servers->begin());
    if (host != _server->getIp())
        return(false);
    return (true);
}

bool        Client::isMethodAllowed()
{
    if (_location->isAllowed(request->getMethod()))
        return (true);
    _statusCode = METHOD_NOT_ALLOWED;
    return (false);
}

