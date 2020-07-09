#include "pch.h"
#define CURL_STATICLIB
#include "WebReq.h"
#include "curl.h"
#include "Util.h"
#include <thread>

//bool WebReq::isSending = false;

WebReq webReq = WebReq();

WebReq::WebReq()
	: isSending(false)
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
}

WebReq::~WebReq() {
	if (sendingThread != nullptr) {
		sendingThread->join();
		delete sendingThread;
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

bool postSend(std::string scriptName, std::string postData, void(*resultCallback)(std::string), WebReq *wr) {
	cout << "Curling to " << webServer << scriptName << " with data = " << postData << endl;

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
		resultCallback(result);
	}

	cout << "Thread terminates" << endl;
	wr->threadFinish();
	return worked;
}



bool WebReq::threadSend(std::string scriptName, std::string postData, void(*resultCallback)(std::string)) {
	if (isSending) {
		cout << "Trying to send but already sending" << endl;
		return false;
	}

	if (sendingThread != nullptr) {
		cout << "Deleting the previous sending thread" << endl;
		sendingThread->join();
		delete sendingThread;
		sendingThread = nullptr;
	}


	isSending = true;
	cout << "Thread begins" << endl;
	sendingThread = new std::thread(postSend, scriptName, postData, resultCallback, this);
}

void WebReq::threadFinish() {
	if (isSending) {
		isSending = false;
	}
	else {
		cout << "WebReq: threadFinish() called but no thread was finishing" << endl;
	}
}
