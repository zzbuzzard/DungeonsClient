#include "pch.h"
#define CURL_STATICLIB
#include "WebReq.h"
#include "CSMode.h"
#ifdef CLIENT
#include "curl.h"
#else
#include "../../Libraries/curl/Debug32/include/curl/curl.h"
#endif
#include "Util.h"
#include <thread>
#include "State.h"


//bool WebReq::isSending = false;

WebReq webReq = WebReq();

WebReq::WebReq()
	//: isSending(false)
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
}

WebReq::~WebReq() {
	for (std::thread *p : deleteThreads) {
		cout << "WebReq destructor deleting a thread" << endl;
		p->join();
		delete p;
	}
	curl_global_cleanup();
}


// To tell the truth, I don't understand this at all.
// Param is assumed to be a pointer to a std::string... Must be the pointer assigned to CURLOPT_WRITEDATA
// but why do we always seem to return size*nmemb in the examples? What is this?
static size_t callback(void* buffer, size_t size, size_t nmemb, void* param)
{
	std::string& text = *static_cast<std::string*>(param);
	size_t totalsize = size * nmemb;
	text.append(static_cast<char*>(buffer), totalsize);
	return totalsize;
}

static const std::string webServer = "https://xmpp.bobinthehob.com/Dungeons/";

bool postSend(std::string scriptName, std::string postData, void(*resultCallback)(ID_t, std::string), ID_t requestID, WebReq *wr, std::thread *thisThread) {
	//cout << "Curling to " << webServer << scriptName << " with data = " << postData << endl;
	webRequests++;

	bool worked = false;

	std::string result;
	CURL* curl;
	CURLcode res;
	curl = curl_easy_init();

	if (curl) {

		std::string url = webServer + scriptName;

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (res == CURLE_OK) {
			worked = true;
		}
		else {
			std::cerr << "CURL error: " << res << '\n';
		}
	}

	if (worked) {
		cout << "Success: calling result callback" << endl;
		resultCallback(requestID, result);
	}
	else
		failedWebRequests++;

	cout << "Thread terminates" << endl;
	wr->threadDone(thisThread);

	return worked;
}



bool WebReq::threadSend(std::string scriptName, std::string postData, void(*resultCallback)(ID_t, std::string), ID_t requestID) {
	//if (isSending) {
	//	cout << "Trying to send but already sending" << endl;
	//	return false;
	//}

	// Todo: the 'delete p;' line once called a std::system_error exception to be thrown. This took about 30 minutes of playing to find tho.
	// I *believe* this is because another thread adding to the std::vector caused the std::vector to reallocate, thus invalidating p.
	//for (std::thread *p : deleteThreads) {
	// Changed to int i system ... this should work.
	//  If this crashes again, try using a std::list.
	//  If this crashes again, the issue will be something else, probably to do with this whole ropey system.

	for (int i=0; i<deleteThreads.size(); i++) {
		cout << "Deleting a thread" << endl;
		deleteThreads[i]->join();
		delete deleteThreads[i];
	}
	deleteThreads.clear();

	//if (sendingThread != nullptr) {
	//	cout << "Deleting the previous sending thread" << endl;
	//	sendingThread->join();
	//	delete sendingThread;
	//	sendingThread = nullptr;
	//}

	// Okay... this is disgusting.
	// The thread's memory is allocated first, then the thread begins
	// It then adds its pointer to our vector once its done so we may deallocate it
	cout << "Thread begins" << endl;
	std::thread *t = new std::thread;
	std::thread *x = t;
	*t = std::thread(postSend, scriptName, postData, resultCallback, requestID, this, x);

	return true;
}

//void WebReq::threadFinish() {
//	if (isSending) {
//		isSending = false;
//	}
//	else {
//		cout << "WebReq: threadFinish() called but no thread was finishing" << endl;
//	}
//}

void WebReq::threadDone(std::thread *t) {
	deleteThreads.push_back(t);
}
