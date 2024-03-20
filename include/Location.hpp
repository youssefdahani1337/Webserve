#pragma once
#include "Macros.hpp"
#include "Configuration.hpp"
class Location
{
private:
		std::string					_ressource;
		std::vector<std::string>	_allowedMethods;
		std::pair<int, std::string>	_redirection;
		bool						_isRedirectable;
		std::string					_root;
		bool						_autoIndex;
		std::vector<std::string>	_index;
		bool						_cgi;
		std::map<std::string, std::string>	_cgiPass;
		std::string					_uploadPath;
    //cgi infos
    
    	std::string					_directiveNames[8];
		void						(Location::*_directiveFcts[8])(std::string &_);
		void	_keyAndValue(std::string line, std::string &key, std::string &value);
		bool	_isLocationDirective(std::string &key);
		void	_fillLocationDirectives(std::map<std::string, std::string> locationDirectives, std::vector<std::string> cgiInfos);
		void	_locationDirectives(std::istringstream &locationBlock);
		void	_ressourceDirective(std::string &value);
		void	_allowedMethodsDirective(std::string &value);
		void	_redirectionDirective(std::string &value);
		void	_rootDirective(std::string &value);
		void	_autoindexDirective(std::string &value);
		void	_indexDirective(std::string &value);
		void	_cgiDirective(std::string &value);
		void	_cgiPassDirective(std::vector<std::string> cgiInfos);
		void	_uploadPathDirective(std::string &value);
		void	_directiveNamesFcts(void);
		void	_copyCgiPass(std::map<std::string, std::string> cgiPassToCopy);

    public:
    	Location(std::string &locationBlock);
    	~Location();
    	Location(Location const &_);
    	Location& operator=(Location const &_);

    	void setRedir(int , std::string);
    	bool isRedir() const ;
    	std::string getRedirPath() const ;
    	int         getRedirCode()  const ;
    	std::string getRootPath();
    	bool isAllowed(const std::string & );
    	std::string getBestIndex(std::string );
    	bool getAutoIndex();
    	bool getCGI();
		std::string getUploadPath() const;
		std::string getRessource() const;
    	std::string isCGIFile(const std::string &  );
};


