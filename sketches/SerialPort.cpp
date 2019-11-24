#include "SerialPort.h"
#include "Board.h"
#include "BrowserServer.h"
#include "XK3118T1.h"

//SerialPortClass SerialPort(UART0);
SerialPortClass * serialPort;


SerialPortClass::SerialPortClass(int port, serial_port_t * value): HardwareSerial(port), Task(200), _value(value) {
	onRun(std::bind(&SerialPortClass::takeWeight, this));
	_authenticated = false;
	unsigned int s = _value->speed;
	unsigned int p = constrain(s, 600, 115200);
#ifndef ESP8266_USE_GDB_STUB
	//end();
	begin(p);
	setTimeout(50);
#endif
	XK3118T1.onEvent([](float data) {
		XK3118T1.detectStable();
	});
}

bool SerialPortClass::canHandle(AsyncWebServerRequest *request) {	
	if (request->url().equalsIgnoreCase(F("/setport.html"))) {
		goto auth;
	}else
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

void SerialPortClass::handleRequest(AsyncWebServerRequest *request) {
	if (request->args() > 0) {
		  // Save Settings
		if(request->hasArg("spd")) {			
			_value->speed = request->arg("spd").toInt();
			_value->accuracy = request->arg("acr").toInt();
			_value->time = request->arg("tme").toInt();
			flush();
			begin(_value->speed);
		}
		if (request->hasArg("user")) {
			request->arg("user").toCharArray(_value->user, request->arg("user").length() + 1);
			request->arg("pass").toCharArray(_value->password, request->arg("pass").length() + 1);
		}
	
		if (Board->memory()->save())
			goto url;
		else
			return request->send(204);
	}
url:
#ifdef HTML_PROGMEM
	request->send_P(200, F("text/html"), setport_html);
#else
	request->send(SPIFFS, request->url());
#endif	
}

void SerialPortClass::handleValue(AsyncWebServerRequest * request) {
	if (!request->authenticate(_value->user, _value->password)) {
		if (!server.checkAdminAuth(request)) {
			return request->requestAuthentication();
		}
	}
	
	AsyncResponseStream *response = request->beginResponseStream(F("application/json"));
	DynamicJsonBuffer jsonBuffer;
	JsonObject &root = jsonBuffer.createObject();
	root["tm_id"] = _value->time;
	root["ac_id"] = _value->accuracy;
	root["sp_id"] = _value->speed;
	root["us_id"] = _value->user;
	root["ps_id"] = _value->password;
	root.printTo(*response);
	request->send(response);
}

void SerialPortClass::takeWeight() {
	DynamicJsonBuffer jsonBuffer;
	JsonObject& json = jsonBuffer.createObject();
	String str = String();
	if (XK3118T1.isSave()) {
		XK3118T1.setIsSave(false);
		json["cmd"] = "swt";
		json["d"] = "";
		json["v"] = String(XK3118T1.get_save_value());
		json["a"] = XK3118T1.getPoint();
	}
	else {		
		json["cmd"] = "wt";
		XK3118T1.doData(json);
	}
	json.printTo(str);
	webSocket.textAll(str);
	serialPort->updateCache();
}


