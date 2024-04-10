#pragma once
#include <queue>
#include <string>
#include <unordered_map>

#include "Message.h"


class Controller
{
	static std::unordered_map<std::string,float (*)(std::string, std::string, void*)> libraries;
	static std::unordered_map<std::string, void*> ciphers;

	std::queue<Message>* in;
	std::queue<Message>* out;

	void processMessage(Message msg);

	void initLibs();

public:
	Controller(std::queue<Message>* in, std::queue<Message>* out);

	void start();
};
