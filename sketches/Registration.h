#pragma once
#include <ESPAsyncWebServer.h>
#include "Config.h"
#include "Task.h"
#include <ArduinoJson.h>

#ifdef HTML_PROGMEM
	#ifndef SOFT_SET
		const char register_html[] PROGMEM = R"()";	
	#endif // SOFT_SET
#endif //HTML_PROGMEM

class RegistrationClass : public AsyncWebHandler{
private:
	settings_t * _value;
public:
	RegistrationClass(settings_t * value) : _value(value) {};
	virtual bool canHandle(AsyncWebServerRequest *request) override final;
	virtual void handleRequest(AsyncWebServerRequest *request) override final;
	virtual bool isRequestHandlerTrivial() override final {return false;};
};

class RegistrationTaskClass : public Task {
protected:
	AsyncWebSocketClient *_client;
	String _payload;
	
public:
	RegistrationTaskClass(JsonObject& json, AsyncWebSocketClient *client): Task(100, true), _client(client) {
		json.printTo(_payload);
	};
	~RegistrationTaskClass() {};
	
	virtual void run() override final;
	
};

class CommissioningTaskClass : public Task {
protected:
	AsyncWebSocketClient *_client;
	String _payload;
	
public:
	CommissioningTaskClass(JsonObject& json, AsyncWebSocketClient *client)	: Task(100, true), _client(client) {
		json.printTo(_payload);
	};
	~CommissioningTaskClass() {};
	
	virtual void run() override final;
	
};

extern RegistrationClass * Registration;