#include "Board.h"
#include "SettingsPage.h"
#include "MultiPointsPage.h"

BoardClass * Board;

BoardClass::BoardClass() {
	_blink = new BlinkClass();
	_memory = new MemoryClass<MyEEPROMStruct>(&_eeprom);
	if (!_memory->init()) {
		doDefault();
	}	
	_wifi = new WiFiModuleClass(&_eeprom);
	serialPort = new SerialPortClass(UART0, &_eeprom.port);
	SettingsPage = new SettingsPageClass(&_eeprom.settings);
#ifdef MULTI_POINTS_CONNECT
	MultiPointsPage = new MultiPointsPageClass(&_eeprom.net, _eeprom.settings.user, _eeprom.settings.password);	
	_wifi->loadPoints();
#endif // MULTI_POINTS_CONNECT
	STAGotIP = WiFi.onStationModeGotIP(std::bind(&BoardClass::onSTAGotIP, this, std::placeholders::_1));	
	stationDisconnected = WiFi.onStationModeDisconnected(std::bind(&BoardClass::onStationDisconnected, this, std::placeholders::_1));
	stationConnected = WiFi.onStationModeConnected(std::bind(&BoardClass::onStationConnected, this, std::placeholders::_1));
};

void BoardClass::init() {
	add(_blink);
	add(_wifi);
	add(serialPort);
};

void /*ICACHE_RAM_ATTR*/ BoardClass::onSTAGotIP(const WiFiEventStationModeGotIP& evt) {
	WiFi.setAutoConnect(true);
	WiFi.setAutoReconnect(true);
	NBNS.begin(WiFi.hostname().c_str());
	onSTA();
}

void BoardClass::onStationConnected(const WiFiEventStationModeConnected& evt) {	
	WiFi.softAP(_eeprom.settings.hostName, SOFT_AP_PASSWORD, evt.channel);    //Устанавливаем канал как роутера
};

void BoardClass::onStationDisconnected(const WiFiEventStationModeDisconnected& evt) {	
	offSTA();
	NBNS.end();	
	webSocket.textAll("{\"cmd\":\"error\",\"ssid\":\"" + evt.ssid + "\",\"status\":\"" + String(evt.reason) + "\"}");
};

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
	_eeprom.port.accuracy = 0;
	_eeprom.port.speed = 9600;
	_eeprom.port.time = 4000;
	u.toCharArray(_eeprom.port.user, u.length() + 1);
	p.toCharArray(_eeprom.port.password, p.length() + 1);
	return _memory->save();
}