#pragma once

#include "Macros.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <netdb.h>

#include "Server.hpp"
#include "Client.hpp"
#include "Configuration.hpp"
class Manager
{
    private:
        std::map<int, std::vector<Server *> > _servers;
        std::map<int, Client *> _clients;

        int maxFd;
        fd_set currentSet, writeSet;
        timeval *time;
    public:
        Manager(int numServers);
		Manager(std::string &configFileName);
        ~Manager();
        Manager(Manager const &_);
        Manager& operator=(Manager const &_);

        void					deleteClient(const int &);
        void					checkTimeOut(const int &);
        
        // this is where i create a network with the outer world
        int                     connectServer(Server *server);
        void                    newConnectionOrRead(int i);
        void					initServers();
        void					multiplexing();
        std::vector<Server *>	*checkifServer(int i);
        void					newConnection(int i, std::vector<Server *> *server);
        void					readRequest(int i);
        void					writeResponse(int i);
        void					addServersToSet();
};
