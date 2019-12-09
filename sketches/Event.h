#pragma once
#include "Config.h"

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "Task.h"
#include "Board.h"


typedef enum EventType {
	EVENT_CONNECT_STA = 0,		/* Соединение с STA */
	EVENT_STABLE_WEIGHT,		/* Стабильный вес */
	EVENT_SEAL_SCALES,			/* Опломбирование весов */
	EVENT_CALIBRATE_COF,		/* Калибровочный коэфициент весы откалиброваные */
	EVENT_UPDATE_LOCAL,			/* Обновление с помощью файла */
	EVENT_UPDATE_GLOBAL,		/* Обновление через интернет на сервере */
	EVENT_LOG,					/* Прочее сообщения */
	EVENT_WEIGHT_AXES			/* Отчет по осям */
} EventType_t;

class EventTaskClass : public Task {
protected:			
	String _hash;	
public:
	EventTaskClass(EventType_t type, String value);
	EventTaskClass(EventType_t type, int value);
	EventTaskClass(EventType_t type, float value);
	~EventTaskClass() {};
	virtual void run() override final;
	String getHash(const String& code, EventType_t type, const String& value);
	//String getHash(const int, const String&, const String&, const String&);
	//String getHash(const String& code, const String& date, const String& type, const String& value);
};