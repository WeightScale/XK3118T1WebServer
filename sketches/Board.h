#pragma once
#include <FS.h>
#include <ESP_EEPROM.h>
#include "Config.h"
#include "Task.h"
#include "TaskController.h"
#include "Battery.h"
#include "WiFiModule.h"
#include "BrowserServer.h"
#include "XK3118T1.h"

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

#ifdef INTERNAL_POWER
class PowerClass : public Task {
private:
	typedef std::function<bool(void)> PowerFunction;
	byte _switch;
	byte _signal;
	PowerFunction _onCallback;
	//void(*onCallback)(void);
	//bool _time_enable = false;
	//unsigned int _time;
	
public : PowerClass(byte swch, byte button_signal, PowerFunction function)	: Task(2400000)	, _switch(swch)	, _signal(button_signal) {
		pinMode(_switch, OUTPUT);
		pinMode(_signal, INPUT);
		_onCallback = function;
		onRun(std::bind(&PowerClass::off, this));
		//POWER.enabled = _settings->power_time_enable;	
		//POWER.setInterval(_settings->time_off);
	};
	~PowerClass() {};
	void begin(unsigned int *time, bool *off) {
		enabled = *off;
		setInterval(*time);
	};
	void on() {digitalWrite(_switch, HIGH); };
	void off() {
		if(_onCallback())
			digitalWrite(_switch, LOW); 
	};
	
	void powerSwitchInterrupt() {
		if (digitalRead(_signal) == HIGH) {
			unsigned long t = millis();
			digitalWrite(LED, LED_ON);
			while (digitalRead(_signal) == HIGH) {
				// 
			   delay(100);	
				if (t + 4000 < millis()) {
					// 
				   digitalWrite(LED, LED_ON);
					while (digitalRead(_signal) == HIGH){delay(10); };// 
					off();			
					break;
				}
				digitalWrite(LED, !digitalRead(LED));
			}
		}
	}
};
#endif // INTERNAL_POWER

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
			digitalWrite(LED, LED_ON);
			clk = 0;
		}
		setInterval(_flash);
	}
	void blinkAP() {
		ledTogle();
		setInterval(500);
	}
	void ledOn() {digitalWrite(LED, LED_OFF); };
	void ledOff() {digitalWrite(LED, LED_ON); };
	void ledTogle() {digitalWrite(LED, !digitalRead(LED)); };
};

class BoardClass : public TaskController {
private:
	struct MyEEPROMStruct _eeprom;
	bool _softConnect = false; /* Флаг соединения softAP */
#ifdef INTERNAL_POWER
	PowerClass *_power;
#endif // INTERNAL_POWER
	BatteryClass *_battery;
	BlinkClass *_blink;
	WiFiModuleClass * _wifi;	
	String _versionSpiffs = "";
	MemoryClass<MyEEPROMStruct> *_memory;
	//BrowserServerClass *_server;
public:
	BoardClass();
	~BoardClass() {
#ifdef INTERNAL_POWER
		delete _power;
#endif // INTERNAL_POWER
		delete _blink;
		delete _memory;
		delete _battery;
	};
	void init();
	void handle() {
		run();
		XK3118T1.handlePort();
#if !defined(POWER_DEBUG) && defined(INTERNAL_POWER)
		_power->powerSwitchInterrupt();		
#endif // !POWER_DEBUG
	};
	BatteryClass * battery() {return _battery;};
	void onSTA() { _softConnect = true; _blink->onRun(std::bind(&BlinkClass::blinkSTA, _blink)); };
	void offSTA() { _softConnect = false; _blink->onRun(std::bind(&BlinkClass::blinkAP, _blink)); };
	bool softConnect() {return _softConnect;};
	void softConnect(bool connect) {_softConnect = connect; };	
	MemoryClass<MyEEPROMStruct> *memory(){return _memory ;};
	WiFiModuleClass * wifi() {return _wifi;};
	void handleBinfo(AsyncWebServerRequest *request);
	void handleAdmin(AsyncWebServerRequest *request);
	void handleCheck(AsyncWebServerRequest *request);			/* determ.html or check.html */
	bool saveEvent(const String& event, float value);
	bool doDefault();
	void loadVersionSpiffs();
	String getVersionSpiff() {return _versionSpiffs;};
#ifdef INTERNAL_POWER
	PowerClass * power() {return _power;};
	void powerOff() {
		webSocket.closeAll();
		server.stop();
		_memory->save();
		_memory->close();
	}
#endif // INTERNAL_POWER

};

extern BoardClass * Board;
void shutDown();