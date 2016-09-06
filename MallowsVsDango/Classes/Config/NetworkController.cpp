#include "NetworkController.h"
#include "cocos2d.h"

USING_NS_CC;

NetworkController::NetworkController(std::string address_server): server(address_server){

}
void NetworkController::sendNewRequest(Request type_request, std::string data,
	const cocos2d::network::ccHttpRequestCallback& callback, std::string tag = "POST") {
	std::string page("/mvd/");
	switch (type_request) {
	case USER_AND_SETTINGS:
		page += "user_handle.php";
		break;
	case LEVEL_EDITOR:
		page += "level_editor_handle.php";
		break;
	case TRACKING:
		page += "tracking_handle.php";
		break;
	case LEVEL_TRACKING:
		page += "level_tracking_handle.php";
		break;
	}

	cocos2d::network::HttpRequest* request = new cocos2d::network::HttpRequest();
	std::string url = server + page;
	request->setUrl(url.c_str());
	request->setRequestType(cocos2d::network::HttpRequest::Type::POST);
	request->setResponseCallback(callback);
	request->setRequestData(data.c_str(), data.length());
	request->setTag(tag.c_str());
	cocos2d::network::HttpClient::getInstance()->send(request);
	request->release();

}