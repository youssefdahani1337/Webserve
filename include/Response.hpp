
#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Message.hpp"

class Response:public Message
{

private:
	unsigned short		_statusCode;
	std::string			_fileName;
	size_t				_fileSize;
	Status				status;
	std::ostringstream	output;
	int					fd;
	bool				_headerCgi ;
	size_t				_lenHeader;
	std::string			logDetails;
public:

	Response();
	Response(const Response& re);
	Response& operator=(const Response& re);
	~Response();

	std::string 	getFile() const ;
	int				getStatus() const;

	void			setFileSize(size_t);
	void			setFile(std::string);
	void			setFd(int);
	void			setLogDetails(std::string);
	void			closeFd();

	void			setStatus(enum Status e);
	void			setheaderCgi(bool, size_t);
	bool			openFile();
	bool			checkReading();
	bool			listDir(std::string , std::string );
	void			generatePage(int );
	void			handleRedir(int , std::string );

	void				generateRequestline();
	void				generateHeaders();
	void				generateResponse(short int statusCode);
	void				makeChunked();
	std::string 		getChunk(bool & endResponse);
	const std::string&	getLogDetails(void) const;
	//debug
	void	printresponse();
};


#endif