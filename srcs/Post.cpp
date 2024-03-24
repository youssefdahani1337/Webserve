#include "../include/Client.hpp"
#include <sstream>
#include <unistd.h>
#include <cstdio>

void Request::parseHexa()
{
    std::string body = getBody();
    
    foundDelimiter = body.find("\r\n", i);
    if (foundDelimiter != std::string::npos)
    {
        std::stringstream str;
        str << body.substr(i, foundDelimiter) << std::hex;
        str >> contentLength;
        i = foundDelimiter + 2;
        if (contentLength != 0)
            gotChunkSize = true;
        else
            endRequest = true;
    }
    else
        throw "BODY";
}

int Request::writeChunkedContent(long long clientMaxBodySize)
{
    std::string s1;
    std::string body = getBody();
    long length = body.length();
    if (contentLength > length)
    {
        s1 = body.substr(i, length);
        contentLength -= (length - i);
        i = length;
    }
    else
    {
        s1 = body.substr(i, contentLength);
        i += contentLength;
        contentLength = 0;
    }
    byte_count += s1.length();
    if (byte_count > clientMaxBodySize)
        return (logDetails = "the total of chunks is larger than the client Max Body Size", CONTENT_TOO_LARGE);
    file.write (s1.c_str(), s1.length());
    return (1);
}

int Request::parseChunkedContent()
{
    std::string body = getBody();

    foundDelimiter = body.find("\r\n", i);
    if (foundDelimiter != std::string::npos)
    {
        gotChunkSize = false;
        i = foundDelimiter + 2;
    }
    else
        throw "body";
    return (1);
}
int Request::parseChunked(long long clientMaxBodySize)
{
    std::string body = getBody();
    size_t length = body.length();
    int status;
    i = 0;
    try
    {
        while (i < length)
        {
            if (!gotChunkSize)
                parseHexa();
            else 
            {
                if ((status = writeChunkedContent(clientMaxBodySize)) != 1)
                    return (status);
                if (contentLength == 0 && (status = parseChunkedContent()) != 1)
                    return (status);
            }
        }
        setBody("");
    }
    catch(const char *str)
    {
        setBody(body.substr(i));
    }
    return (1);
}

int Request::addToFile(long long clientMaxBodySize)
{
    std::string body = getBody();   
    long length = body.length();
    int status;
 
    if (!chunked)
    {
        if (contentLength - length < 0)
        {
            file.write(body.c_str(), contentLength);
            contentLength = 0;
        }
        else
        {
           file.write(body.c_str(), length);
           contentLength -= length;  
        }
        if (!contentLength)
            endRequest = true;
        setBody("");
    }
    else if (chunked && (status = parseChunked(clientMaxBodySize)) != 1)
        return (status);
    return (1);
}
int Request::initFile(std::string &path, long long clientMaxBodySize, bool cgiResponse)
{
    std::string value;
    std::stringstream random;
    char                *ptr;

    if ((value = getHeaderValue("transfer-encoding")) != "")
    {
        if (value != "chunked")
            return (logDetails = "transfer-encoding != chunked", NOT_IMPLEMENTED);
        chunked = true;
    }
    else if ((value = getHeaderValue("content-length")) != "")
    {
        contentLength = strtod(value.c_str(), &ptr);
        if (contentLength < 0 || *ptr)
            return (logDetails = "the value of contentLength is invalid", BAD_REQUEST);
        byte_count = contentLength;
    }
    else
        return (logDetails = "we don't have a contentLength and its not chunked", LENGTH_REQUIRED);
    if (contentLength > clientMaxBodySize)
        return (logDetails = "contentLength is larger than the client Max Body size", CONTENT_TOO_LARGE);
    try
    {
        random << path << "/file" << rand() << Tools::identity++;
        if (!cgiResponse && getBody().length())
            random << Tools::findExtension(getHeaderValue("content-type"));
        fileName = random.str();
        file.open(fileName.c_str());
        if (file.is_open())
            fileCreated = true;
    }
    catch (int statusCode)
    {
        return (std::remove(fileName.c_str()), statusCode);
    }
    return (1);
}
int Client::initPost()
{
    int status;
    std::string uploadStore = _location -> getUploadPath();
    if (uploadStore != "")
    {
        if (Tools::checkIfPathValid(uploadStore))
        {
            if ((status = request -> initFile(uploadStore, _server -> getMaxBodySize(), _cgiResponse)) != 1)
                return (status);
        }
        else
            return(request ->setLogDetails("we don't have access to the upload path"), FORBIDDEN);
    }
    else
    {
        if (_location -> getCGI())
        {
            _cgiResponse = true;
            std::string pathToTmp = TMPDIR;
            Tools::checkIfPathValid(pathToTmp);
            if ((status = request -> initFile(pathToTmp, _server -> getMaxBodySize(), _cgiResponse)) != 1)
                return (status);
        }
        else
        {
            request ->setLogDetails("we dont have an uploadPath and no cgi");
            return (FORBIDDEN);
        }
    }
    return (1);
}

int Client::PostHandler()
{
    int status;

    if (_location && _location->isRedir())
       return (_location->getRedirCode());
    if (!request -> getFileCreated())
    {
        if ((status = initPost()) != 1)
        {
            return (std::remove(request -> getFileName().c_str()),
            Tools::updateLogFile(status, request -> getMethod() , _server, request -> getLogDetails()), _cgiResponse = false,  status);
        }
    }
    if ((status = request -> addToFile(_server -> getMaxBodySize())) != 1)
        return (std::remove(request -> getFileName().c_str()),
            Tools::updateLogFile(status, request -> getMethod() , _server, request -> getLogDetails()), _cgiResponse = false, status);
    if (request -> getEndRequest())
    {
        request -> setFile();
        request -> setLogDetails("the file was uploaded successfully");
        std::string fileName = request -> getFileName();
        return (CREATED);
    }
    return (1);
}
