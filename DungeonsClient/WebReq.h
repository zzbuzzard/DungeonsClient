#pragma once
#include "Types.h"
//#include <functional>

namespace std {
	class thread;
}

class WebReq
{
public:
	WebReq();
	~WebReq();

	bool threadSend(std::string scriptName, std::string postData, void(*callback)(std::string));
	void threadFinish(); // internal only
private:
	bool isSending;
	std::thread *sendingThread;
	// Takes a script name, the data to be POSTed, and a callback function pointer
	// bool postSend(std::string scriptName, std::string postData, void(*callback)(std::string));
};

extern WebReq webReq;

