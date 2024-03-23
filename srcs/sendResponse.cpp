#include "../include/Client.hpp"

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
    fcntl(_fdClient, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
    if (-1 == write(_fdClient, msg.c_str(), msg.length()))
    {
        Tools::updateLogFile(0,request->getMethod() ,_server, "write fail in send");
        return (false);
    }
    if (!endresponse)
    {
        if (response->getStatus() == CGI_FILE)
          remove(response->getFile().c_str());
        Tools::updateLogFile(_statusCode, request->getMethod(), _server, response->getLogDetails());
    }
    return (endresponse);
}