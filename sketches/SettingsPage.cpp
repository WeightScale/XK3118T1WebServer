#include "SettingsPage.h"
#include "Board.h"
#include "BrowserServer.h"

SettingsPageClass * SettingsPage;

bool SettingsPageClass::canHandle(AsyncWebServerRequest *request) {	
	if (request->url().equalsIgnoreCase(F("/settings.html"))) {
		goto auth;
	}
#ifndef HTML_PROGMEM
	else if (request->url().equalsIgnoreCase("/sn")) {
		goto auth;
	}
#endif
	else
		return false;
auth:
	if (!request->authenticate(_value->user, _value->password)) {
		if (!server.checkAdminAuth(request)) {
			request->requestAuthentication();
			return false;
		}
	}
	return true;
}

/*String SettingsPageClass::processor(const String& var) {
	if (var == F("assid"))
		return String(_value->hostName);
	else if(var == F("nadmin"))
		return String(_value->user);
	else if(var == F("padmin"))
		return String(_value->password);
	else if(var == F("local_host"))
		return WiFi.hostname();
	else if(var == F("net"))
		return WiFi.SSID();
	else if(var == F("sta_ip"))
		return WiFi.localIP().toString();
	else if(var == F("ap_ip"))
		return WiFi.softAPIP().toString();
	else if(var == F("mac"))
		return WiFi.macAddress();
	else if(var == F("vr"))
		return PRODUCT;
	return String();
}*/

void SettingsPageClass::handleRequest(AsyncWebServerRequest *request) {
	if (request->args() > 0) {
		String message = " ";
		if (request->hasArg("assid")) {	
			request->arg("assid").toCharArray(_value->hostName, request->arg("assid").length() + 1);
			request->arg("nadmin").toCharArray(_value->user, request->arg("nadmin").length() + 1);
			request->arg("padmin").toCharArray(_value->password, request->arg("padmin").length() + 1);
			
			if (Board->memory()->save())
				goto url;
			else
				return request->send(400);
		}		
		return request->send(204);
	}
url:
#ifdef HTML_PROGMEM
	//request->send_P(200, F("text/html"), settings_html, std::bind(&SettingsPageClass::processor, this, std::placeholders::_1));
	request->send_P(200, F("text/html"), settings_html);
#else
	if (request->url().equalsIgnoreCase("/sn"))
		request->send_P(200, F("text/html"), netIndex);
	else
		request->send(SPIFFS, request->url(),String(),false,std::bind(&SettingsPageClass::processor, this, std::placeholders::_1));
#endif
}

/**/void SettingsPageClass::handleValue(AsyncWebServerRequest * request) {
	if (!request->authenticate(_value->user, _value->password)) {
		if (!server.checkAdminAuth(request)) {
			return request->requestAuthentication();
		}
	}
	
	AsyncResponseStream *response = request->beginResponseStream(F("application/json"));
	DynamicJsonBuffer jsonBuffer;
	JsonObject &root = jsonBuffer.createObject();
	doSettings(root);	
	root.printTo(*response);
	request->send(response);
}

size_t SettingsPageClass::doSettings(JsonObject &root) {
	JsonObject& scale = root.createNestedObject(SCALE_JSON);

	scale["id_assid"] = _value->hostName;
	scale["id_nadmin"] = _value->user;
	scale["id_padmin"] = _value->password;
	
	JsonObject& info = root.createNestedObject("info");
	
	info["id_local_host"] = WiFi.hostname();
	info["id_net"] = WiFi.SSID();
	info["id_sta_ip"] = WiFi.localIP().toString();
	info["id_ap_ip"] = WiFi.softAPIP().toString();
	info["id_mac"] = WiFi.macAddress();
	info["id_vr"] = PRODUCT;
	return root.measureLength();
};