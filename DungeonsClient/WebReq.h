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

	// Starts a new thread which sends a web request. When the web request completes,
	//  It calls resultCallback(requestID, string returned by web request)
	bool threadSend(std::string scriptName, std::string postData, void(*resultCallback)(ID_t, std::string), ID_t requestID);
	void threadDone(std::thread *t); // internal only
private:
	std::vector<std::thread*> deleteThreads;

	//bool isSending;
	//std::thread *sendingThread;
	// Takes a script name, the data to be POSTed, and a callback function pointer
	// bool postSend(std::string scriptName, std::string postData, void(*callback)(std::string));
};

extern WebReq webReq;

