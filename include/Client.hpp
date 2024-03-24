#pragma once

#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include <sys/stat.h>

//_path the match (location::root + request->resource(url))
//Request::_resource (uri)
// Response::_fileName the matche _path or the path of the file errorPage


class Response;
class Request;

class Client
{
private:
    int                  _fdClient;
    std::vector<Server *> *_servers;
    Server                *_server;
    Location            *_location;
    std::string         _cgiPath;
    Request             *request;
    Response            *response;
    
    int                 _statusCode;
    pid_t               pid;
    ParseReq_St         status;

    std::string         buffer;
    std::string         _path;

    time_t               _lastTime;
    time_t               _firstTime;
    bool                _cgiResponse;
    bool                _startSend;

    std::string         parseHost(std::string );
    bool                whichServer();
    bool                checkPath();
    bool                getResPath();
    bool                isPost();
    void                checkResource();

    void                readBuffer();
    void                handleDirectory();
    void                handleAutoIndex();
    void                handleRedir();
    bool                handleCGI();


    bool                checkCGI();
    void                cgiProcess(std::string tmpFile);
    bool                serverProcess();
    void                runCgi();
    void                parseCgiFile();
    char                **setEnv(mapStrStr);
    char                **FillEnv();
    void                buildErrorPage();
    bool                isMethodAllowed();
	int					DeleteDirectory(std::string dirPath);
    void	            dirContent(DIR *dir, std::string dirPath, int &status);
    bool                checkAndgenerate();
    void                sendHeadError();


public:


    Client (int fd_client, std::vector<Server *> *server);
    ~Client();

    bool                    outOfTimeRead();
    bool                    outOfTimeSend();
    void                    timeOut();
    std::vector<Server *>   *getServers() const;
    Server *                getServer() const ;
    bool                    parseRequest();//read and parse 
    const int &             getErrorCode(void) const;
    bool                    sendResponse();
    bool	                handleRequest();
    bool                    handleResponse();
    bool                    isDelete();
    int                     DeleteHandler();
    int 			        PostHandler();
    int                     initPost();

    void                    setStatusCode(int);
};

// make all getters as a reference.

