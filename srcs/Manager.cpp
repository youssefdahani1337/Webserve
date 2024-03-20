#include "../include/Manager.hpp"

std::ofstream Tools::logFile;
Manager::Manager(std::string &configFileName) : maxFd(0)
{
    Tools::setTypes();
    this->_servers = Configuration::parseConfigFile(configFileName);
    time = new timeval;
    time -> tv_sec = 2;
    time -> tv_usec = 0;
    Tools::logFile.open("logs/logFile.txt");
    if (!Tools::logFile.is_open())
        throw std::runtime_error("problem while opening the logFile");
}

std::vector<Server *> *Manager::checkifServer(int i)
{
    std::map<int, std::vector<Server *> >::iterator its;
    std::vector<Server *> empty;

    its = _servers.find(i);
    return (its != _servers.end() ? &(its->second) : NULL);
}
int Manager::connectServer(Server *server)
{
    addrinfo hints, *res, *rep;
    int serverFd;
    int enable = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    if (getaddrinfo(server->getIp().c_str(), server-> getPort().c_str(), &hints, &res))
    {
        freeaddrinfo(res);
        throw std::runtime_error("getaddrinfo error");
    }
    for (rep = res; rep != NULL; rep = rep -> ai_next)
    {
        if ((serverFd = socket(res -> ai_family, res -> ai_socktype, res -> ai_protocol)) != -1)
            break;
    }
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    if (bind(serverFd, reinterpret_cast<sockaddr *>(res ->ai_addr), res ->ai_addrlen) == -1)
    {
        freeaddrinfo(res);
        throw std::runtime_error("port num : " + server -> getPort() + " is reserved");
    }
    listen(serverFd, 100);
    std::cout << "server num " << serverFd << " is connected on ip " << server->getIp()
        << " and port " << server -> getPort() << std::endl; 
    freeaddrinfo(res);
    return (serverFd);
}
void Manager::initServers()
{
    std::map<int, std::vector<Server *> >::iterator its, itsend;
    std::map<int, std::vector<Server *> >::iterator tmp;
    Server *server;
    int serverFd;

    its = _servers.begin();
    itsend = _servers.end();
    while (its != itsend && its -> first < 0)
    {
        server = *(its -> second.begin());
        serverFd = connectServer(server);
        tmp = its;
        its++;
        server -> setServerFd(serverFd);
        _servers[serverFd] = tmp -> second;
        _servers.erase(tmp);
    }
    multiplexing();
}

void Manager::newConnection(int i, std::vector<Server *> *server)
{
    int clientFd;
    sockaddr_in *client = new sockaddr_in();// where is delete for this alloc
    socklen_t length = sizeof(*client);
    if ((clientFd = accept(i, reinterpret_cast<sockaddr *>(client), &length)) == -1)
    {
        delete client;
        return ;
    }
    FD_SET(clientFd, &(this -> currentSet));
    if (clientFd > this ->maxFd)
        this ->maxFd = clientFd;
    this -> _clients[clientFd] = new Client(clientFd, server);
    delete client;
}
void Manager::readRequest(int i)
{
    try
    {
        if (!_clients[i]->handleRequest())
        {
            FD_CLR(i, &(this -> currentSet));
            FD_SET(i, &(this -> writeSet));
        }
    }
    catch(const std::exception& e)
    {
       deleteClient(i);
    }
} 

void Manager::writeResponse(int i)
{  
    if (_clients[i] && !_clients[i]->sendResponse())
        deleteClient(i);
}

void  Manager::checkTimeOut(const int & i)
{
    if (FD_ISSET(i, &currentSet) && _clients[i] &&  _clients[i]->outOfTimeRead())
    {
        _clients[i]->setStatusCode(REQUEST_TIMEOUT);
        _clients[i]->handleResponse();

        FD_CLR(i, &(this -> currentSet));
        FD_SET(i, &(this -> writeSet)); // why do u need to set it to this -> writeSet
    }
    else if (FD_ISSET(i, &writeSet) && _clients[i] &&  _clients[i]->outOfTimeSend())    
        deleteClient(i);
}

void    Manager::deleteClient(const int & i)
{
    close(i);
    if (FD_ISSET(i, &this -> writeSet))
        FD_CLR(i, &(this -> writeSet));
    else
        FD_CLR(i, &(this -> currentSet));
    delete _clients[i];
    _clients[i] = NULL;
    this->_clients.erase(i);
}

void Manager::addServersToSet()
{
    std::map<int, std::vector<Server *> >::iterator its, itsend;
    int serverFd;
    
    FD_ZERO(&(this -> currentSet));
    FD_ZERO(&(this -> writeSet));
    its = _servers.begin();
    itsend = _servers.end();
    while (its != itsend)
    {
        serverFd = its -> first;
        FD_SET(serverFd, &(this -> currentSet));
        if (serverFd > this ->maxFd)
            this ->maxFd = serverFd;
        its++;
    }
}
void Manager::newConnectionOrRead(int i)
{
    std::vector<Server *> *server;
    
    server = checkifServer(i);
    if (server)
        newConnection(i, server);
    else
        readRequest(i);
}
void Manager::multiplexing()
{
    fd_set readSet, writeSetTmp;
    int i;
    
    addServersToSet(); 
    while (true)
    {
        readSet = this -> currentSet;
        writeSetTmp = this -> writeSet;
        i = -1;
        if (select(this ->maxFd + 1, &readSet, &writeSetTmp, NULL, time) < 0)
            throw std::runtime_error("Select Error");
        while (++i <= this ->maxFd)
        {
            if (FD_ISSET(i, &readSet))
                newConnectionOrRead(i);
            else if (FD_ISSET(i, &writeSetTmp))
                writeResponse(i);
            else if ((FD_ISSET(i, &writeSet) || FD_ISSET(i, &currentSet)) && !checkifServer(i))
                checkTimeOut(i);
        }
    }
}

Manager::~Manager()
{
    std::vector<Server *>::iterator it, itend;
    std::map<int, std::vector<Server *> >::iterator its, itsend;
    std::map<int, Client *>::iterator itc, itendc;

    its = _servers.begin();
    itsend = _servers.end();

    itc = _clients.begin();
    itendc = _clients.end();
    while (its != itsend)
    {
        for (size_t i = 0; i < its -> second.size(); i++)
            delete its->second.at(i);
        its++;
    }
    while (itc != itendc)
    {
        delete itc->second;
        itc ++;
    }
    Tools::logFile.close();
    delete time;
}
