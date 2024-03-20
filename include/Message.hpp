#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include "Macros.hpp"

class Message
{
protected:
	const std::string _version;
	std::string _body;
	std::map<std::string, std::string> _headers;
	std::string messageContent;

public:

	Message();
	//AMessage();
	Message(const Message& );
	Message& operator=(const Message& );
	
	virtual ~Message();

	const std::string& getBody(void) const;
	void setBody(const std::string&);
	void addBody(std::string);

	std::string  getHeaderValue(const std::string& );
	bool addHeader(std::string, std::string);
	
	void printHeader();

};



#endif