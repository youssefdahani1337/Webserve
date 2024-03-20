#pragma once

# include "Location.hpp"

class Location;
class Server
{
    private:
    	int								_serverFd;
		std::string            			_host;
        std::string                     _port;
        std::vector<std::string>  	    _serverNames;
        std::map<int, std::vector<std::string> >	    _errorPages;
		size_t   				    	_maxBodySize;
        std::vector<Location *>		    _locations;


		std::string					_directiveNames[4];
		void						(Server::*_directiveFcts[4])(std::string &_);
		void						_checkStartingPoint(std::string &line);
        void						_keyAndValue(std::string line, std::string &key, std::string &value);
        bool						_isServerDirective(std::string &key);
        void						_fillServerDirectives(std::map<std::string, std::string> serverDirectives, std::vector<std::string> errorPages);
		void						_listen(std::string &value);
		void						_hostDirective(std::string &value);
		void						_serverNamesDirective(std::string &value);
		void						_errorPagesDirective(std::vector<std::string> &errorPages);
		void					    _maxBodySizeDirective(std::string &value);
		void						_serverDirectives(std::istringstream &blockIss);
		template <typename T> std::vector<T>	_copyVec(std::vector<T> src);
		void						_copyErrPages(std::map<int, std::vector<std::string> > otherErrPages);
		void						_directiveNamesFcts(void);
		void						_checkDuplicateLocation(std::string ressource);
        void                        sortLocation();

    public:
		Server();
        ~Server();
        Server(Server const &_);
        Server& operator=(Server const &_);
        Server(std::string &serverBlock, std::vector<std::string> &locations);

        int             getServerFd() const;
        std::string     getPort() const;
        std::string     getIp() const;
        long long       getMaxBodySize() const;
		std::vector<std::string>	getServerNames() const;
        // mapStrStr &     getType();
        // std::string     getMimeType(const std::string & );
        Location*       getMatchLocation(std::string , std::string &);
        std::string     getFileError(int errorCode);
        
        void            setServerFd(int ServerFd);
        // static void     setTypes();
};