#ifndef MACROS_HPP
#define MACROS_HPP

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <string>
#include <stdio.h>
#include <string.h>
#include <fstream> 
#include <limits.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>


#include<algorithm>
#include <vector>
#include <map>

#include <fcntl.h>
#include "Tools.hpp"

#include <ctime>
#include <cmath>

enum Status
{
	begin_RES,
	REDIR,
	DIRECTORY,
	ERROR,
	IS_FILE,
	READING_FILE,
	CGI_ERROR,
	CGI_RUNNING,
	CGI_FILE,
	STREAM,
	STREAM_FILE,

};


enum ParseReq_St
{
	begin,
	ReadReqL,
	ReadHeaders,
	ReadBody
};

#define	MAX_SERVERS 10

#define FINISH 0
#define NOT_YET 1
#define INVALID_CHARS "\" `<>{}|\\^~"
#define UNRESERVED_CHARS "-._~"
#define MAX_HEADERS 100
#define MAX_BUFFER_HEADER 1024

#define TIME_OUT 10
#define TIME_OUT_UPLOAD 3
#define MAX_TIME_OUT 10
#define TIME_OUT_SEND 20
#define MAX_TIME_OUT_SEND 10
#define BUFFER_SIZE 1024
#define CGI_TIME 5


#define TMPDIR "./tmp/Webserve"

#define SUCCESS 200
#define CREATED 201
#define NO_CONTENT 204 //

#define MOVED_PERMANENTLY 301
#define BAD_REQUEST		400
#define FORBIDDEN		403
#define NOT_FOUND		404
#define METHOD_NOT_ALLOWED 405
#define REQUEST_TIMEOUT 408
#define CONFLICT		409 //
#define LENGTH_REQUIRED 411

#define CONTENT_TOO_LARGE 413
#define URI_TOO_LONG	414
#define UNSUPPORTED_MEDIA_TYPE 415
#define REQUEST_HEADER_FIELDS_TOO_LARGE 431

#define INTERNAL_SERVER_ERROR 500
#define NOT_IMPLEMENTED 501
#define BAD_GATEWAY 502
#define GATEWAY_TIMEOUT 504
#define HTTP_VERSION_NOT_SUPPORTED 505
#endif