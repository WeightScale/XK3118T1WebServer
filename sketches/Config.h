#pragma once

#define VERSION(plate,name, num) plate name num

//#define WEB_TERMINAL2			"web_2_"
//#define WEB_TERMINAL_MINI		"web_mini_"
//#define WEB_CRANE				"web_crane_"	
#define SCALE_SERVER			"scale_server_"


//#define DEBUG_SERIAL
#define DEBUG_GDB
//#define POWER_DEBUG
#define HTML_PROGMEM			//Использовать веб страницы из flash памяти
#define MULTI_POINTS_CONNECT	/* Использовать для использования с несколькими точками доступа */


#ifdef DEBUG_SERIAL
#define POWER_DEBUG
#define DEBUG_BATTERY		/*Для теста*/
#ifdef DEBUG_ESP_PORT
#define DEBUG_BOARD(...) DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#endif
#elif defined(DEBUG_GDB)
#define POWER_DEBUG
#define DEBUG_BATTERY		/*Для теста*/
#define ESP8266_USE_GDB_STUB    //Comment out the definition below if you don't want to use the ESP8266 gdb stub.
#endif

#ifndef DEBUG_BOARD
#define DEBUG_BOARD(...)
#endif

//#define MY_HOST_NAME "scl"
//#define SOFT_AP_SSID "SCALES"
#define SOFT_AP_PASSWORD "12345678"
#define HOST_URL "sdb.net.ua"		/** Адрес базы данных*/

#define TIMEOUT_HTTP 3000			/** Время ожидания ответа HTTP запраса*/
#define STABLE_NUM_MAX 10			/** Количество стабильных измерений*/
#define MAX_EVENTS 100				/** Количество записей событий*/


#ifdef  WEB_TERMINAL2
#include "web_terminal2.h"
#define INTERNAL_POWER
#define PLAN_BATTERY			BATTERY_4V
#define SKETCH_VERSION			"web_terminal2.001"
#define SPIFFS_VERSION			SKETCH_VERSION
#elif defined( WEB_TERMINAL_MINI)
#include "web_terminal_mini.h"
#define EXTERNAL_POWER
	//#define PLAN_BATTERY			BATTERY_6V	
#define SKETCH_VERSION			"web_terminal_mini.001"
#define SPIFFS_VERSION			SKETCH_VERSION
#elif defined(WEB_CRANE)
#include "web_crane.h"
#define INTERNAL_POWER
#define PLAN_BATTERY			BATTERY_4V
#define SKETCH_VERSION			"web_crane.001"
#define SPIFFS_VERSION			SKETCH_VERSION
#elif defined(SCALE_SERVER)
#include "scale_server.h"
#define SCALES_AXES				/* Поосное взвешивание */
#define INTERNAL_POWER
#define PLATE					SCALE_SERVER
#define NUM_VRS					"012b"
#ifdef SOFT_SET
#ifdef ZERO_SET
#define NAME			"soft_8266_4m_z."
#else
#define NAME			"soft_8266_4m."
#endif // ZERO_SET
#else
#ifdef ZERO_SET
#define NAME			"8266_4m_z."
#else			
#define NAME			"8266_4m."			
#endif // ZERO_SET
#endif // SOFT_SET
#endif //SCALE_SERVER

#define PRODUCT				VERSION(PLATE, NAME, NUM_VRS)

#ifdef HTML_PROGMEM
#include "Page.h"
#endif
#ifdef SCALES_AXES
#include "AxesClass.h"
#endif // SCALES_AXES

typedef struct {
	char hostUrl[0xff];
	int hostPin;
	unsigned int numCheck;
}admin_t;

typedef struct {
#ifdef MULTI_POINTS_CONNECT
	unsigned int timeScan;  		//Время период сканирования в секундах
	unsigned char deltaRSSI;  	//Дельта мощности сигнала при проверке в процентах
	bool enable_scan; 			//Использовать сканирование лучшей сети из списка
#else
	bool dnip;	
	char lanIp[16];
	char gate[16];
	char mask[16];
	char wSSID[33];
	char wKey[33];
#endif // !MULTI_POINTS_CONNECT	
}net_t;

typedef struct {
	unsigned long speed;
	unsigned int time;
	//int accuracy;
	float startDetermine;
	char user[16];
	char password[16];
} serial_port_t;

typedef struct {
#ifdef INTERNAL_POWER
	bool power_time_enable;
	unsigned int time_off;
#endif
	char hostName[16];
	char unit[5];
	char user[16];
	char password[16];
	unsigned int bat_max; /* максимальный заряд батареи */
	unsigned int bat_min; /* минимальный заряд батареи */
} settings_t;

struct MyEEPROMStruct {
	net_t net;
	settings_t settings;
	serial_port_t port;
	admin_t admin;
};

//#define MAX_CHG 1013//980	//делитель U2=U*(R2/(R1+R2)) 0.25
//#define MIN_CHG 880			//ADC = (Vin * 1024)/Vref  Vref = 1V
//#define MIN_CHG 670			//ADC = (Vin * 1024)/Vref  Vref = 1V	Vin = 0.75 5.5v-6.5v
//#define MIN_CHG 500			//ADC = (Vin * 1024)/Vref  Vref = 1V  Vin = 0.49v  3.5v-4.3v

/*#if POWER_PLAN == INTERNAL_POWER
	#define MIN_CHG 500			//ADC = (Vin * 1024)/Vref  Vref = 1V  Vin = 0.49v  3.5v-4.3v
#else if POWER_PLAN == EXTERNAL_POWER
	#define MIN_CHG 820			//ADC = (Vin * 1024)/Vref  Vref = 1V	Vin = 0.75 5.5v-6.5v
#endif*/

/* для удаленного обновления
Формат файла
[имя].[версия].[тип].[расширение]
имя			- имя файла или мак адресс
версия		- версия программы число
тип			- тип файла ino или spiff
расширение	- bin
пример: weight_scale.001.spiff.bin
алгоритм проверки приблизительно такой
делаем split
если не bin false
если тип spiff или ino выбираем соответствуюшую папку типа spiffs sketch 
в папке ищем папку по имени и проверяем версию
хидеры которые отправляет код
    http.useHTTP10(true);
    http.setTimeout(8000);
    http.setUserAgent(F("ESP8266-http-Update"));
    http.addHeader(F("x-ESP8266-STA-MAC"), WiFi.macAddress());
    http.addHeader(F("x-ESP8266-AP-MAC"), WiFi.softAPmacAddress());
    http.addHeader(F("x-ESP8266-free-space"), String(ESP.getFreeSketchSpace()));
    http.addHeader(F("x-ESP8266-sketch-size"), String(ESP.getSketchSize()));
    http.addHeader(F("x-ESP8266-sketch-md5"), String(ESP.getSketchMD5()));
    http.addHeader(F("x-ESP8266-chip-size"), String(ESP.getFlashChipRealSize()));
    http.addHeader(F("x-ESP8266-sdk-version"), ESP.getSdkVersion());
    if(spiffs) {
	    http.addHeader(F("x-ESP8266-mode"), F("spiffs"));
	    } else {
	    http.addHeader(F("x-ESP8266-mode"), F("sketch"));
    }
    if(currentVersion && currentVersion[0] != 0x00) {
	    http.addHeader(F("x-ESP8266-version"), currentVersion);
    }
    const char * headerkeys[] = { "x-MD5" };
    size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
    // track these headers
    http.collectHeaders(headerkeys, headerkeyssize);
*/