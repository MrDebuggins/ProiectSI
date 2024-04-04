#pragma once
#include <string>


class Message
{
public:
	std::string message;
	void* obj;

	Message(std::string message, void* obj)
	{
		this->message = message;
		this->obj = obj;
	}
};
