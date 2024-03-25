#include "../include/Client.hpp"

Client::Client(int fd_client, std::vector<Server *> *server)
: _fdClient(fd_client),  _servers(server), _location (NULL),  request(new Request()), 
response(new Response()), _statusCode(0), _cgiResponse(false)
{
    response->setStatus(begin_RES);
    status = begin;
    _server = NULL;
    _lastTime = Tools::getTime();
    _firstTime = _lastTime;
    pid = -2;
    _startSend = false;
}

std::vector<Server *> *Client::getServers() const {return (this -> _servers);}

Server * Client::getServer() const {return (this -> _server);}

const int & Client::getErrorCode(void) const { return (this->_statusCode);}

void            Client::setStatusCode(int code) { _statusCode = code;}
bool            Client::isPost() { return (request->getMethod() == "POST" ? 1 : 0);}

bool            Client::isDelete() { return (request->getMethod() == "DELETE" ? 1 : 0);}

bool	Client::handleRequest()
{
    if (!outOfTimeRead(false) && parseRequest())
            return (true);
    if (_statusCode > 299)
        Tools::updateLogFile(_statusCode,request->getMethod(), _server, request->getLogDetails());
    return (handleResponse());
}



bool Client::outOfTimeSend()
{
    if ((Tools::getTime() - _firstTime) > TIME_OUT_SEND)
    {
        Tools::updateLogFile(0,request->getMethod() ,_server, "Time out in send");
        return (true);
    }
    _firstTime = Tools::getTime();
    return (false);
}

bool    Client::outOfTimeRead(bool out)
{
    _lastTime = Tools::getTime();
    
    if ((_lastTime - _firstTime) > 5)
    {
        _statusCode = REQUEST_TIMEOUT;
        request->setFile();
        remove(request->getFileName().c_str());
        Tools::updateLogFile(_statusCode,request->getMethod(), _server, "While reading request");
        return (true);
    }
    if (!out)
        _firstTime =  Tools::getTime();
    return (false);
}

Client::~Client()
{
    delete this->_location;
    delete this->request;
    delete this->response;
}
