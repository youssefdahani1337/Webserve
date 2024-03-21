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
    if (!outOfTimeRead() && parseRequest())
            return (true);
    if (_statusCode > 299)
        Tools::updateLogFile(_statusCode,request->getMethod(), _server, request->getLogDetails());
    return (handleResponse());
}

bool    Client::checkAndgenerate()
{
    if (response->getStatus() == CGI_RUNNING && !serverProcess())
        return (true);
    handleCGI();
    if (response->getStatus() == READING_FILE || response->getStatus() == CGI_FILE )
        response->openFile();
    response->generateResponse(_statusCode);
    _startSend = true;
    _firstTime = Tools::getTime();
    _lastTime = _firstTime;
    return (false);
}

bool        Client::sendResponse()
{
    bool            endresponse;
    std::string     msg;


    endresponse = true;
    if (!_startSend && checkAndgenerate())
        return (true);
    if (outOfTimeSend())
        return(false);
    msg = response->getChunk(endresponse);
    fcntl(_fdClient, F_SETFL, O_NONBLOCK, FD_CLOEXEC); // THIS ARE THE ONLY OPTIONS I CAN USE
    if (-1 == write(_fdClient, msg.c_str(), msg.length()))
    {
        Tools::updateLogFile(0,request->getMethod() ,_server, "write fail in send");
        return (false);
    }
    if (!endresponse)
    {
        if (response->getStatus() == CGI_FILE)
           remove(response->getFile().c_str());
        Tools::updateLogFile(_statusCode, request->getMethod(), _server, request->getLogDetails());
    }
    return (endresponse);
}

bool Client::outOfTimeSend()
{
    if ((Tools::getTime() - _firstTime) > TIME_OUT_SEND)
    {
        Tools::updateLogFile(0,request->getMethod() ,_server, "Time out in send");
        return (true);
    }
    return (false);
}

bool    Client::outOfTimeRead()
{
    _lastTime = Tools::getTime();
    if (!isPost() && MAX_TIME_OUT != 0 && (_lastTime - _firstTime) > MAX_TIME_OUT)
    {
        _statusCode = REQUEST_TIMEOUT;
        Tools::updateLogFile(_statusCode,request->getMethod(), _server, "While reading request ");
        return (true);
    }
    else if ((isPost() && ((TIME_OUT_UPLOAD != 0) && (_lastTime - _firstTime) > TIME_OUT_UPLOAD)))
    {
        _statusCode = REQUEST_TIMEOUT;
        request->setFile();
        remove(request->getFileName().c_str());
        Tools::updateLogFile(_statusCode,request->getMethod(), _server, "While reading request");
        return (true);
    }
    return (false);
}

Client::~Client()
{
    delete this->_location;
    delete this->request;
    delete this->response;
}
