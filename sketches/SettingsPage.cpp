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

void SettingsPageClass::handleRequest(AsyncWebServerRequest *request) {
	if (request->args() > 0) {
		String message = " ";
		if (request->hasArg("host")) {	
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
	request->send_P(200, F("text/html"), settings_html);
#else
	if (request->url().equalsIgnoreCase("/sn"))
		request->send_P(200, F("text/html"), netIndex);
	else
		request->send(SPIFFS, request->url());
#endif
}

void SettingsPageClass::handleValue(AsyncWebServerRequest * request) {
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
	info["id_vr"] = SKETCH_VERSION;
	return root.measureLength();
};