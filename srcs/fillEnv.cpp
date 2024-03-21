#include "../include/Client.hpp"

char ** Client::setEnv(mapStrStr envMap)
{
	char											**env;
	std::string										envVar;
	std::map<std::string, std::string>::iterator	it;
	int												i;

	i = -1;
	env = new char*[envMap.size() + 1];
	it = envMap.begin();
	while (it != envMap.end())
	{
		envVar = it->first + "=" + it->second;
		env[++i] = strdup(envVar.c_str());
		it++;
	}
    env[++i] = NULL;
	return (env);
}

char **Client::FillEnv()
{
	std::map<std::string, std::string>	envVars;
	std::stringstream	ss;

	ss << this->request->getContentLength();
	envVars.insert(std::make_pair("CONTENT_LENGTH", ss.str()));
	ss.clear();
	envVars.insert(std::make_pair("CONTENT_TYPE",this->request->getHeaderValue("content-type")));
	envVars.insert(std::make_pair("GATEWAY_INTERFACE", "CGI/1.1")); 
	envVars.insert(std::make_pair("QUERY_STRING", this->request->getQuery()));
    envVars.insert(std::make_pair("REQUEST_METHOD", this->request->getMethod()));
	envVars.insert(std::make_pair("SCRIPT_NAME", Tools::findFileName(response->getFile())));
    envVars.insert(std::make_pair("SCRIPT_FILENAME", Tools::realPath(response->getFile()) ));
	envVars.insert(std::make_pair("SERVER_PORT",_server->getPort()));
	envVars.insert(std::make_pair("SERVER_PROTOCOL", "HTTP/1.1"));
	envVars.insert(std::make_pair("SERVER_SOFTWARE", "WebWaiters"));
	envVars.insert(std::make_pair("PATH_INFO", Tools::realPath(response->getFile())));
	envVars.insert(std::make_pair("REDIRECT_STATUS", "200")); //????
	envVars.insert(std::make_pair("HTTP_COOKIE", this->request->getHeaderValue("cookie")));
	envVars.insert(std::make_pair("HTTP_HOST", this->request->getHeaderValue("host")));
    return (this->setEnv(envVars));
}