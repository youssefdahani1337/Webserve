#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Message.hpp"

class Request:public Message
{

private:
	std::string 	_method;
	std::string 	_resource;
	std::string 	_uri;
	std::string 	_query;
	std::string		_fragment;
	bool			endRequest;
	unsigned short	headerCount;
	std::string		logDetails;


	//private attribute that i have added for post method
	bool			fileCreated;
	bool			chunked;
	bool			gotChunkSize;
	size_t			foundDelimiter;
	std::ofstream	file;
	std::string		fileName;
	long			contentLength;
	long long   	byte_count; //how many byte is read
	size_t			i;
	bool			parseUri(int & code);
	bool			uri_decode();
	
public:

	Request();
	Request(const Request& re);
	Request& operator=(const Request& re);
	~Request();

	void				setMethod(std::string);
	void				setEndRequest();
	void				setResource(std::string);
	void				setFile();
	void				setLogDetails(std::string str);

	const std::string&	getMethod(void) const ;
	const std::string&	getLogDetails(void) const;
	bool 				getEndRequest();
	std::string			getResource(void)  ;
	bool				getFileCreated();
	const std::string&	getFileName()const;	
	long				getContentLength();
	std::string			getQuery() const;
	std::string			getUri() const;
	int					parseRequestLine(std::string & , ParseReq_St & );
	int					parseHeader(std::string & , ParseReq_St &);
	void				showRequest();

	//all of those funtions that i have added for post
	int 	initFile(std::string & , long long );
	int 	addToFile(long long );
	void	parseHexa();
	int 	writeChunkedContent(long long );
	int 	parseChunked(long long );
	int		parseChunkedContent();
	// Debug
	void	printHeaderH();
};
#endif