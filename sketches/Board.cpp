#include "Board.h"
#include "SettingsPage.h"
#include "MultiPointsPage.h"

BoardClass * Board;

BoardClass::BoardClass() {
	_blink = new BlinkClass();
#ifdef INTERNAL_POWER
	//_power = new PowerClass(EN_NCP, PWR_SW, std::bind(&BoardClass::powerOff, this));
	_power = new PowerClass(EN_NCP, PWR_SW, [](){/* Событие выключения питания */
		Board->powerOff();
	});	
	auto startTime = millis();
	_blink->ledOn();
	// wait for on power
	while((millis() - startTime) <= 2000) {
		delay(100);
	}
	_power->on();
	_blink->ledOff();
	add(_power);
#endif
	_memory = new MemoryClass<MyEEPROMStruct>(&_eeprom);
	if (!_memory->init()) {
		doDefault();
	}	
	_battery = new BatteryClass(&_eeprom.settings.bat_min, &_eeprom.settings.bat_max);
	_wifi = new WiFiModuleClass(&_eeprom);
	serialPort = new SerialPortClass(UART0, &_eeprom.port);
	SettingsPage = new SettingsPageClass(&_eeprom.settings);
#ifdef MULTI_POINTS_CONNECT
	MultiPointsPage = new MultiPointsPageClass(&_eeprom.net, _eeprom.settings.user, _eeprom.settings.password);	
	_wifi->loadPoints();
#endif // MULTI_POINTS_CONNECT	
};

void BoardClass::init() {
#ifdef INTERNAL_POWER
	_power->begin(&_eeprom.settings.time_off, &_eeprom.settings.power_time_enable);
#endif
	add(_blink);
	add(_battery);
	add(_wifi);
	add(serialPort);
	/* События для отправки сообщения о разряде батареи */
	_battery->onEventDischarged([](unsigned char charge) {
		webSocket.textAll("{\"cmd\":\"dchg\"}");
		String msg = "Батарея разряжена ";
		msg += String(charge) + "%";
		Board->add(new EventTaskClass(LOG, msg));
		Board->add(new Task(shutDown, 120000));
	});
	/* События для отправки значения заряда */
	_battery->onEvent([](unsigned char charge) {
		DynamicJsonBuffer jsonBuffer;
		JsonObject& json = jsonBuffer.createObject();
		String str = String();
		Board->battery()->doData(json);
		json.printTo(str) ;
		webSocket.textAll(str) ;
	});
	_battery->fetchCharge();
	/*if (_battery->isDischarged()) {
		
	}*/
	_wifi->onEventConnectSTA([](bool status) {
		if (status){
			Board->onSTA();
		}else{
			Board->offSTA();
		}
	});
};

void BoardClass::handleBinfo(AsyncWebServerRequest *request) {
	if (!request->authenticate(_eeprom.settings.user, _eeprom.settings.password))
		if (!server.checkAdminAuth(request)) {
			return request->requestAuthentication();
		}
	if (request->args() > 0) {
		bool flag = false;
		if (request->hasArg("bmax")) {
			float t = request->arg("bmax").toFloat();
			_eeprom.settings.bat_max = CONVERT_V_TO_ADC(t);
			flag = true;
		}
		if (flag) {
			if (request->hasArg("bmin")) {
				float t = request->arg("bmin").toFloat();
				_eeprom.settings.bat_min = CONVERT_V_TO_ADC(t);
			}
			else {
				flag = false;
			}				
		}
		if (flag && _memory->save()) {
			goto url;
		}
		return request->send(400);
	}
url:
	request->send(SPIFFS, request->url());
}

bool BoardClass::doDefault() {
	String u = F("admin");
	String p = F("1234");
	String apSsid = F("SCALES");
	u.toCharArray(_eeprom.settings.user, u.length() + 1);
	p.toCharArray(_eeprom.settings.password, p.length() + 1);
	apSsid.toCharArray(_eeprom.settings.hostName, apSsid.length() + 1);
	_eeprom.settings.hostPin = 0;
#ifndef MULTI_POINTS_CONNECT
	_eeprom.settings.dnip = true;
#else
	_eeprom.net.timeScan = 20;
	_eeprom.net.deltaRSSI = 20;
	_eeprom.net.enable_scan = true;
#endif // MULTI_POINTS_CONNECT
	//_eeprom.port.accuracy = 0;
	_eeprom.port.speed = 9600;
	_eeprom.port.time = 4000;
	_eeprom.port.startDetermine = 200;
	u.toCharArray(_eeprom.port.user, u.length() + 1);
	p.toCharArray(_eeprom.port.password, p.length() + 1);
	return _memory->save();
}

void shutDown() {	
	Board->power()->off();
}