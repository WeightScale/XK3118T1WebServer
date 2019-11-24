#include "HttpUpdater.h"
#include "Board.h"
#include "StreamString.h"
#include <ESP8266httpUpdate.h>
#include "Version.h"

HttpUpdaterClass httpUpdater;
String updaterError;
int command;

HttpUpdaterClass::HttpUpdaterClass(const String& username, const String& password)
:_username(username),_password(password),_authenticated(false)
{}

bool HttpUpdaterClass::canHandle(AsyncWebServerRequest *request){
	if(request->url().equalsIgnoreCase("/update")){
		return true;
	}
	return false;
}

void HttpUpdaterClass::handleRequest(AsyncWebServerRequest *request){
	if(_username.length() && _password.length() && !request->authenticate(_username.c_str(), _password.c_str()))
		return request->requestAuthentication();
	_authenticated = true;
	if(request->method() == HTTP_GET){
		request->send_P(200, F("text/html"), serverIndex);	
	}else if (request->method()==HTTP_POST){
		digitalWrite(2, LOW); //led off
		if (command == U_SPIFFS){
			Board->memory()->save();
			request->redirect("/");
			return;
		}
		if(updaterError && updaterError[0] != 0x00){
			AsyncWebServerResponse * response = request->beginResponse(200, "text/html", updaterError);
			request->send(response);
		}else{
			AsyncWebServerResponse * response = request->beginResponse_P(200, "text/html", successResponse);
			response->addHeader("Connection", "close");
			request->send(response);
			request->onDisconnect([](){ESP.reset();});
		}
	}
}

void HttpUpdaterClass::handleUpload(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
	digitalWrite(LED, !digitalRead(LED));	//led on
	
	if(!index){
		updaterError = String();
		if(!_authenticated){
			updaterError = F("filed authenticated");
			return;
		}
		size_t size;
		if(filename.indexOf(F("spiffs.bin"),0) != -1) {
			command = U_SPIFFS;
			size = ((size_t) &_SPIFFS_end - (size_t) &_SPIFFS_start);
		}else if(filename.indexOf("ino.bin",0) != -1) {
			command = U_FLASH;
			size = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
		}else{
			updaterError = F("error file");
			return;
		}
		
		Update.runAsync(true);
		if(!Update.begin(size, command)){
			setUpdaterError();
		}
	}
	if(!Update.hasError()){
		//digitalWrite(LED, !digitalRead(LED));	//led on
		if(Update.write(data, len) != len){
			setUpdaterError();
		}
	}
	if(final){
		if(!Update.end(true)){
			setUpdaterError();
		}
	}
}

void HttpUpdaterClass::setUpdaterError(){
	StreamString str;
	Update.printError(str);
	updaterError = str.c_str();
}

void HttpUpdaterClass::handleHttpStartUpdate(AsyncWebServerRequest * request){										/* Обновление чере интернет address/hu?host=sdb.net.ua/update.php */
	if(!request->authenticate(_username.c_str(), _password.c_str()))
		return request->requestAuthentication();
	if(request->hasArg("host")){
		String host = request->arg("host");
		//_server->send(200, "text/plain", host);
		ESPhttpUpdate.rebootOnUpdate(false);
		digitalWrite(LED, LED_OFF);
		String url = String("http://");
		url += host;
		t_httpUpdate_return ret = ESPhttpUpdate.updateSpiffs(url,PRODUCT);
		if (ret == HTTP_UPDATE_OK){
			Board->memory()->save();
			ret = ESPhttpUpdate.update(url, PRODUCT);
		}
		switch(ret) {
			case HTTP_UPDATE_FAILED:
				request->send(404, F("text/plain"), ESPhttpUpdate.getLastErrorString());
			break;
			case HTTP_UPDATE_NO_UPDATES:
				request->send(304, F("text/plain"), F("Обновление не требуется"));
			break;
			case HTTP_UPDATE_OK:
				request->client()->stop();
				ESP.restart();
			break;
		}
		
	}
	digitalWrite(LED, LED_ON);
};
	