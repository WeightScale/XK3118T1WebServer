#pragma once
#include <FS.h>
#include "TaskController.h"
#include "Config.h"
#include <functional>
#include <ESP_EEPROM.h>
#include "BrowserServer.h"
#include "WiFiModule.h"
#include "SerialPort.h"

#define SCALE_JSON		"scale"
#define SERVER_JSON		"server"

template <typename T>class MemoryClass : protected EEPROMClass {
public:
	T * _value;

public:
	MemoryClass(T *mem) {_value = mem;};
	~MemoryClass() {close();};
	bool init() {
		SPIFFS.begin();	
		begin(sizeof(T));
		if (percentUsed() < 0)
			return false;
		get(0, *_value);
		return true;
	};
	bool save() {
		put(0, *_value);
		return commit();
	};
	void close() {
		end();
		SPIFFS.end();
	};
	bool doDefault();	
};

class BlinkClass : public Task {
public:
	unsigned int _flash = 500;
public:
	BlinkClass(): Task(500) {
		pinMode(LED, OUTPUT);
		ledOn();
		onRun(std::bind(&BlinkClass::blinkAP, this));
	};
	void blinkSTA() {
		static unsigned char clk;
		bool led = !digitalRead(LED);
		digitalWrite(LED, led);
		if (clk < 6) {
			led ? _flash = 70 : _flash = 40;
			clk++;
		}else {
			_flash = 2000;
			digitalWrite(LED, HIGH);
			clk = 0;
		}
		setInterval(_flash);
	}
	void blinkAP() {
		ledTogle();
		setInterval(500);
	}
	void ledOn() {digitalWrite(LED, LOW); };
	void ledOff() {digitalWrite(LED, HIGH); };
	void ledTogle() {digitalWrite(LED, !digitalRead(LED)); };
};

class BoardClass : public TaskController {
private:
	struct MyEEPROMStruct _eeprom;
	BlinkClass *_blink;
	WiFiModuleClass * _wifi;
	WiFiEventHandler stationConnected;
	WiFiEventHandler stationDisconnected;
	WiFiEventHandler STAGotIP;
	MemoryClass<MyEEPROMStruct> *_memory;
	BrowserServerClass *_server;
public:
	BoardClass();
	~BoardClass() {
		delete _blink;
		delete _memory;
	};
	void init();
	void handle() {
		run();	
	};
	void onSTA() {_blink->onRun(std::bind(&BlinkClass::blinkSTA, _blink)); };
	void offSTA() {_blink->onRun(std::bind(&BlinkClass::blinkAP, _blink)); };
	void onStationConnected(const WiFiEventStationModeConnected& evt);
	void onStationDisconnected(const WiFiEventStationModeDisconnected& evt);
	void onSTAGotIP(const WiFiEventStationModeGotIP& evt);
	MemoryClass<MyEEPROMStruct> *memory(){return _memory ;};
	WiFiModuleClass * wifi() {return _wifi;};
	bool saveEvent(const String& event, float value);
	bool doDefault();	
};

extern BoardClass * Board;