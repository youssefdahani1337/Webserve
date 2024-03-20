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

void    Client::cgiProcess(std::string tmpFile)
{
    char    *path[3];
    int     fd;
    char    **env;
    env     = this->FillEnv();
    path[0] = strdup(this->_cgiPath.c_str());
    path[1] = strdup(Tools::realPath(response->getFile()).c_str());
    path[2] = NULL;

    fd = open(tmpFile.c_str(), O_CREAT | O_RDWR, 0777); //check
    if (fd == -1)
        exit(1);
    if (dup2(fd, STDOUT_FILENO) == -1)
        exit(2);
    if (dup2(fd, STDERR_FILENO) == -1)
        exit(2);
    execve(path[0], path, env);
    exit(3);
}

bool    Client::serverProcess()
{
    int	status;
    int result;

    result = waitpid(pid, &status, WNOHANG);
    if (result > 0)
    {
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
            _statusCode = SUCCESS;
        else
            _statusCode = BAD_GATEWAY;
    }
    else if (result == 0)
    {
        if (Tools::getTime() - _firstTime < CGI_TIME)
            return (0);
        if (kill(pid, SIGTERM) == -1)
            kill(pid, SIGKILL);
        _statusCode = GATEWAY_TIMEOUT;
    }
    else if (result == -1)
        _statusCode = INTERNAL_SERVER_ERROR;
    
    this->response->setStatus((_statusCode == SUCCESS) ? CGI_FILE : CGI_ERROR);
    if (this->isPost())
        remove(this->request->getFileName().c_str());
    if (response->getStatus() == CGI_ERROR)
        remove(this->response->getFile().c_str());
    return (1);
}

void    Client::runCgi()
{
    std::string tmpFile;

    tmpFile = Tools::RandomFile();
    
    pid = fork();
    if (tmpFile.empty() || pid == -1)
    {
        request->setLogDetails("can't open tmp file");
        _statusCode = INTERNAL_SERVER_ERROR;
        this->response->setStatus(CGI_ERROR);
        return;
    }
    else if (pid == 0)
        this->cgiProcess(tmpFile);
    else
    {
        this->response->setFile(tmpFile);
        this->response->setStatus(CGI_RUNNING);
        _firstTime = Tools::getTime();
    }
    return ;
}

void    Client::parseCgiFile()
{
    std::ifstream   infile;
    std::istringstream iss;
    std::string     line, tmpLine, str;
    size_t          len = 0;
    size_t          strLen;
    size_t          lenHeader = 0;

    infile.open(response->getFile().c_str());
    if (!infile.is_open())
    {
        response->setStatus(CGI_ERROR);
        _statusCode = INTERNAL_SERVER_ERROR;
        return ;
    }
    _statusCode = SUCCESS;
    while (std::getline(infile, line) && !infile.eof())
    {
        lenHeader += line.length() + 1;
        if (++len > MAX_HEADERS)
                break ;
        if (line == "\r")
        {
            response->setheaderCgi(true, lenHeader);
            break ;
        }
        strLen = line.length();
        if (strLen > 1 && strLen < MAX_BUFFER_HEADER && line.at(line.size() - 1) == '\r')
        {
            iss.str(line);
            tmpLine.clear();
            if (getline(iss, tmpLine, ':') && !iss.eof())
            {
                tmpLine = Tools::toUpper(tmpLine);
                iss >>  std::ws;
                iss >> str;
                if (tmpLine == "status")
                    _statusCode = strtol(str.c_str(), NULL, 10);
                iss.clear();
            }
            else
                break;
        }
            continue;
        break;
    }
    infile.close();
	return ;
}


bool    Client::handleCGI()
{
    if (response->getStatus() == CGI_FILE)
        parseCgiFile();
    if (response->getStatus() == CGI_ERROR  )
    {
        buildErrorPage();
        return (false);
    }
    return (true);
}