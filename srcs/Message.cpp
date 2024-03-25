#include "../include/Message.hpp"

Message::Message():_version("HTTP/1.1"),_body("")
{

}

Message::Message(const Message& message)
{
	*this = message;
}
Message& Message::operator=(const Message& message)
{
	if (this != &message)
	{
		this->_body = message._body;
		this->_headers = message._headers;
	}
	return (*this);
}

Message::~Message(){}

const std::string& Message::getBody(void) const
{
	return (this->_body);
}
void Message::setBody(const std::string& body)
{
	this->_body = body;
}
void Message::addBody(std::string bodyPart)
{
	this->_body += bodyPart;
}
std::string  Message::getHeaderValue(const std::string& key)
{
	std::map<std::string, std::string>::iterator it ;

	it = _headers.find(key);
	return (it != _headers.end() ? it->second : "");
}
bool  Message::addHeader(std::string key, std::string value)
{
	return (_headers.insert(std::pair<std::string , std::string> (key, value)).second);
}
