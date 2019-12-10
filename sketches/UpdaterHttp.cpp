#include "UpdaterHttp.h"
#include "Board.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

UpdaterHttpClass * UpdaterHttp;

UpdaterHttpClass::UpdaterHttpClass(): _username("sa"), _password("654321") {}

UpdaterHttpClass::UpdaterHttpClass(const String& username, const String& password): _username(username), _password(password){}

bool UpdaterHttpClass::canHandle(AsyncWebServerRequest *request) {
	if (request->url().equalsIgnoreCase("/check")) {
		//if(request->url().equalsIgnoreCase("/test.html")){
			return true;
	}
	return false;
}

void UpdaterHttpClass::handleRequest(AsyncWebServerRequest *request) {
	if (_username.length() && _password.length() && !request->authenticate(_username.c_str(), _password.c_str()))
		return request->requestAuthentication();
	
#ifdef HTML_PROGMEM
	request->send_P(200, F("text/html"), check_html);
#else
	request->send(SPIFFS, request->url());
#endif
}

void /*ICACHE_RAM_ATTR*/ CheckTaskClass::run() {
#ifdef SOFT_SET
	//todo команда на soft
#else
	if (!Board->softConnect())
		return;
	HTTPClient http;
	String url = "http://";
	url += Board->memory()->_value->admin.hostUrl;
	url += "/update.php?check";
	/**/
	http.begin(url);
	http.useHTTP10(true);
	http.setTimeout(8000);
	http.addHeader(F("x-ESP8266-AP-MAC"), WiFi.softAPmacAddress());
	http.addHeader(F("x-ESP8266-product"), String(String(PLATE) + NAME));
	http.addHeader(F("x-ESP8266-spiffs"), String(Board->getVersionSpiff()));
	http.addHeader(F("x-ESP8266-ino"), F(NUM_VRS));
	int code = (t_http_codes)http.GET();
	String msg = String();
	DynamicJsonBuffer jsonBuffer;
	if (code == HTTP_CODE_OK) {		
		JsonObject &json = jsonBuffer.parseObject(http.getString());
		json["cmd"] = "update";
		json["code"] = code;
		json.printTo(msg);
	}else {		
		JsonObject &json = jsonBuffer.createObject();
		json["cmd"] = "update";
		json["code"] = code;
		json.printTo(msg);
	}		
	http.end();
	_client->text(msg);
#endif // SOFT_SET

	
}

void /*ICACHE_RAM_ATTR*/ UpdateTaskClass::run() {
#ifdef SOFT_SET
	//todo команда на soft
#else
	Update.runAsync(true);
	Update.onProgress([this](size_t progress, size_t size) {			
		/**/DynamicJsonBuffer jsonBuffer;
		JsonObject &json = jsonBuffer.createObject();
		json["cmd"] = "prog";
		json["total"] = size;
		json["loaded"] = progress;
		String msg = String();
		json.printTo(msg);
		_client->text(msg);
		digitalWrite(LED, !digitalRead(LED));
	});
	ESPhttpUpdate.rebootOnUpdate(false);
	String url = "http://";
	url += Board->memory()->_value->admin.hostUrl;
	url += "/update.php?update";
	t_httpUpdate_return ret = HTTP_UPDATE_FAILED;
	if (_type == SPIFFS_TYPE){
		ret = ESPhttpUpdate.updateSpiffs(url, _file);		
	}else if (_type == INO_TYPE){
		ret = ESPhttpUpdate.update(url, _file);
	}else{
		return;
	}
	DynamicJsonBuffer jsonBuffer;
	JsonObject &json = jsonBuffer.createObject();
	json["cmd"] = "update";
	json["code"] = (int)ret;
	String msg = String();
	json.printTo(msg);
	_client->text(msg);
	_type = UNKNOW_TYPE;
	_file = "";
#endif // SOFT_SET
}