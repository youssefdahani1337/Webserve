#include "../include/Client.hpp"

void    Client::cgiProcess(std::string tmpFile)
{
    char    *path[3];
    // int     fd;
    char    **env;

    env     = this->FillEnv();
    path[0] = strdup(this->_cgiPath.c_str());
    path[1] = strdup(Tools::realPath(response->getFile()).c_str());
    path[2] = NULL;
    if (isPost())
    {
        if (!freopen(this->request->getFileName().c_str(), "r", stdin))
            exit(1);
    }
    if (!freopen(tmpFile.c_str(), "w+", stdout))
        exit(1);
    // if (dup2(fd, STDERR_FILENO) == -1)
    //     exit(1);
    execve(path[0], path, env);
    exit(1);
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
        {
            request->setLogDetails("Bad Gateway");
            _statusCode = BAD_GATEWAY;
        }
    }
    else if (result == 0)
    {
        if (Tools::getTime() - _firstTime < CGI_TIME)
            return (0);
        if (kill(pid, SIGTERM) == -1)
            kill(pid, SIGKILL);
        _statusCode = GATEWAY_TIMEOUT;

        //scanf("%d", &result);
        request->setLogDetails("time out in cgi");
    }
    else if (result == -1)
    {
        _statusCode = INTERNAL_SERVER_ERROR;
        request->setLogDetails("result -1 in cgi");
    }
    
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
        request->setLogDetails("can't open output file of cgi");
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
    if (response->getStatus() == CGI_ERROR)
    {
        buildErrorPage();
        return (false);
    }
    return (true);
}