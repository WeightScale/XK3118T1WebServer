#include "Registration.h"
#include "BrowserServer.h"
#include <ESP8266HTTPClient.h>
#include "Board.h"

RegistrationClass * Registration;

bool RegistrationClass::canHandle(AsyncWebServerRequest *request) {
	if (request->url().equalsIgnoreCase(F("/register.html"))) {
		if (!request->authenticate(_value->user, _value->password)) {
			if (!server.checkAdminAuth(request)) {
				request->requestAuthentication();
				return false;
			}
		}
		return true;
	}else
		return false;
}

void RegistrationClass::handleRequest(AsyncWebServerRequest *request) {
	request->send(SPIFFS, request->url());
}

void RegistrationTaskClass::run(){
#ifdef SOFT_SET
	//todo команда на soft
#else
	if (!Board->softConnect())
		return;
	HTTPClient http;
	String url = "http://";
	url += Board->memory()->_value->admin.hostUrl;
	url += "/register.php?register=";
	url += _payload;
	http.begin(url);
	http.useHTTP10(true);
	http.setTimeout(8000);
	http.addHeader(F("x-ESP8266-SPIFFS"), String(Board->getVersionSpiff()));
	http.addHeader(F("x-ESP8266-INO"), PRODUCT);
	http.addHeader(F("x-ESP8266-AP-MAC"), WiFi.softAPmacAddress());
	http.addHeader("Content-Type", "application/json");
	int code = (t_http_codes)http.GET();
	String msg = String();
	DynamicJsonBuffer jsonBuffer;
	if (code == HTTP_CODE_OK) {
		JsonObject &json = jsonBuffer.parseObject(http.getString());
		json["cmd"] = "status";
		json["code"] = code;
		json.printTo(msg);
	}else {
		//DynamicJsonBuffer jsonBuffer;
		JsonObject &json = jsonBuffer.createObject();
		json["cmd"] = "status";
		json["code"] = code;
		json.printTo(msg);
	}		
	http.end();
	_client->text(msg);
#endif // SOFT_SET

	
}

void CommissioningTaskClass::run() {	
#ifdef SOFT_SET
	//todo команда на soft
#else
	if (!Board->softConnect()){
		return;
	}
	HTTPClient http;
	String url = "http://";
	url += Board->memory()->_value->admin.hostUrl;
	url += "/register.php?comm=";
	url += _payload;
	http.begin(url);
	http.useHTTP10(true);
	http.setTimeout(5000);
	http.addHeader(F("x-ESP8266-SPIFFS"), String(Board->getVersionSpiff()));
	http.addHeader(F("x-ESP8266-INO"), PRODUCT);
	http.addHeader(F("x-ESP8266-AP-MAC"), WiFi.softAPmacAddress());
	http.addHeader("Content-Type", "application/json");
	int code = (t_http_codes)http.GET();
	String msg = String();
	DynamicJsonBuffer jsonBuffer;
	if (code == HTTP_CODE_OK) {
		JsonObject &json = jsonBuffer.parseObject(http.getString());
		json["cmd"] = "status";
		json["code"] = code;
		json.printTo(msg);
	}else {
		//DynamicJsonBuffer jsonBuffer;
		JsonObject &json = jsonBuffer.createObject();
		json["cmd"] = "status";
		json["code"] = code;
		json.printTo(msg);
	}		
	http.end();
	_client->text(msg);
#endif // !SOFT_SET
}