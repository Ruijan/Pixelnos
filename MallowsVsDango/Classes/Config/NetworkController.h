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
		TRACKING,
		LEVEL_TRACKING
	};
	/** 
	* @brief Constructor of the network.
	* @param the actual adress of the server.
	*/
	NetworkController(std::string address_server);
	/**
	* @brief Send a new request to the webpage
	* @param type_request: USER_AND_SETTINGS, LEVEL_EDITOR, TRACKING, LEVEL_TRACKING
	* @param data: check the php page to see what you have to send for each request.
	* @param callback: what do you want to do when the server will send an answer. Use a lambda function.
	* @param tag: if you want to check what request you sent, this tag will be accessible through the answer.
	* example: network_controller->sendNewRequest(NetworkController::Request::USER_AND_SETTINGS, "action=create_user&name=ruijan&id_game=" + rootSav["id_player"].asString(),
	*		[&, this](cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response) {
	*		waiting_answer = false;
	*		if (response->isSucceed()) {
	*		}
	*		else {
	*			log("request error");
	*		}
	*	}, "POST createUser");
	*/
	void sendNewRequest(Request type_request, std::string data,
		const cocos2d::network::ccHttpRequestCallback& callback, std::string tag);

};

#endif
