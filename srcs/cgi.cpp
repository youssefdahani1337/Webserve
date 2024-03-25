#include "../include/Client.hpp"

void    Client::cgiProcess(std::string tmpFile)
{
    char        *path[3];
    char        **env;
    struct stat st;

    env = this->FillEnv();
    path[0] = strdup(this->_cgiPath.c_str());
    path[1] = strdup(Tools::realPath(response->getFile()).c_str());
    path[2] = NULL;
    if (isPost())
    {
        if (!freopen(this->request->getFileName().c_str(), "r", stdin))
            return ;
    }
    if (!freopen(tmpFile.c_str(), "w+", stdout))
        return ;
    if (stat(path[0], &st)!= 0 && !(st.st_mode & S_IXUSR))
        return ;
    if (access(path[1], R_OK) == -1)
        return ;
    if (dup2(Tools::fdError, STDERR_FILENO) == -1)
        return ;
    execve(path[0], path, env);
    return ;
}

bool    Client::serverProcess()
{
    int	status;
    int result;

    result = waitpid(pid, &status, WNOHANG);
    if (result > 0)
    {
        if (WIFEXITED(status))
        {
            _statusCode = SUCCESS;
            std::cout << WEXITSTATUS(status) << std::endl;
        }
        else
        {
            request->setLogDetails("Bad Gateway");
            response->setStatus(CGI_ERROR);
            _statusCode = BAD_GATEWAY;
            parseCgiFile();
        }
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
    {
        _statusCode = INTERNAL_SERVER_ERROR;
        request->setLogDetails("waipid failed in cgi");
    }
    this->response->setStatus((_statusCode == SUCCESS) ? CGI_FILE : CGI_ERROR);
    if (response->getStatus() == CGI_ERROR)
       remove(this->response->getFile().c_str());
    return (1);
}

void    Client::runCgi()
{
    std::string tmpFile;

    tmpFile = Tools::RandomFile();
    pid = fork();
    if (tmpFile.empty() || access(tmpFile.c_str(), F_OK) == 0 || pid == -1)
    {
        request->setLogDetails("can't open tmp file");
        _statusCode = INTERNAL_SERVER_ERROR;
        this->response->setStatus(CGI_ERROR);
        return;
    }
    else if (pid == 0)
    {
        this->cgiProcess(tmpFile);
        pid = getpid();
        if (kill(pid, SIGTERM) == -1)
            kill(pid, SIGKILL);
    }
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
    short int       code;


    if ((isPost() && access(request->getFileName().c_str(), F_OK | R_OK) != 0) || 
        access(response->getFile().c_str(), F_OK | W_OK) != 0)
    {
        _statusCode = INTERNAL_SERVER_ERROR;
        response->setStatus(CGI_ERROR);
        return ;
    }

    infile.open(response->getFile().c_str());
    if (!infile.is_open())
    {
        response->setStatus(CGI_ERROR);
        request->setLogDetails("can't open output file of cgi");
        _statusCode = INTERNAL_SERVER_ERROR;
        return ;
    }
    if (response->getStatus() != CGI_ERROR)
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
                iss >> code;
                if (tmpLine == "status" && !iss.fail())
                {
                    if (code >= 400 && code <= 599)
                    {
                     _statusCode = code;
                        response->setStatus(CGI_ERROR);
                        infile.close();
                        return ;
                    }
                }
                iss.clear();
            }
        }
        else
            break;
        continue;
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
    if (response->getStatus() == CGI_FILE || response->getStatus() == CGI_ERROR)
    {
          if (this->isPost() )
            remove(this->request->getFileName().c_str());
    }
    return (true);
}