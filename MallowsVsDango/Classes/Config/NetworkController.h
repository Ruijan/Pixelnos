#ifndef NETWORKCONTROLLER_HPP
#define NETWORKCONTROLLER_HPP

#include <iostream>
#include "network/HttpClient.h"

class NetworkController{
	
private:
	std::string server;
	
public:
	enum Request {
		USER_AND_SETTINGS,
		LEVEL_EDITOR,
		TRACKING
	};
	NetworkController(std::string address_server);
	void sendNewRequest(Request type_request, std::string data,
		const cocos2d::network::ccHttpRequestCallback& callback, std::string tag);

};

#endif
