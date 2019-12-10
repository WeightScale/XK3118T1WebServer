#include "Event.h"
#include <ESP8266WiFi.h>
#include "Board.h"
#include <ESP8266HTTPClient.h>

EventTaskClass::EventTaskClass(EventType_t type, String value)	: Task(100, true) {
	_hash = getHash(WiFi.softAPmacAddress(), type, value);
	_try = 0;
};
EventTaskClass::EventTaskClass(EventType_t type, int value)	: Task(100, true) {
	_hash = getHash(WiFi.softAPmacAddress(), type, String(value));	
	_try = 0;
};

EventTaskClass::EventTaskClass(EventType_t type, float value) : Task(100, true) {
	_hash = getHash(WiFi.softAPmacAddress(), type, String(value));	
	_try = 0;
};

void /*ICACHE_RAM_ATTR*/ EventTaskClass::run() {
#ifdef SOFT_SET
	//todo команда на soft
#else
	if (!Board->softConnect())
		return;
	HTTPClient http;
	String message = "http://";
	message += Board->memory()->_value->admin.hostUrl;	
	message += "/scales.php?hash=" + _hash;
	http.begin(message);
	http.setTimeout(5000);
	int httpCode = http.GET();
	http.end();
	_try++;
	if (httpCode == HTTP_CODE_OK){
		single(true);	
	}else{
		setInterval(2000);
		single(_try > TRY_COUNT);		
	}
#endif // !SOFT_SET
};

String EventTaskClass::getHash(const String& code, EventType_t type, const String& value) {	
	String event = String(code);
	event += "\n" + String(type) + "\n" + value;
	int s = 0;
	for (int i = 0; i < event.length(); i++)
		s += event[i];
	event += (char)(s % 256);
	String hash = "";
	for (int i = 0; i < event.length(); i++) {
		int c = (event[i] - (i == 0 ? 0 : event[i - 1]) + 256) % 256;
		int c1 = c / 16; int c2 = c % 16;
		char d1 = c1 < 10 ? '0' + c1 : 'a' + c1 - 10;
		char d2 = c2 < 10 ? '0' + c2 : 'a' + c2 - 10;
		hash += "%"; hash += d1; hash += d2;
	} 
	return hash;
}






/*String EventTaskClass::getHash(const String& code, const String& date, const String& type, const String& value) {	
	String event = String(code);
	event += "\t" + date + "\t" + type + "\t" + value;
	int s = 0;
	for (int i = 0; i < event.length(); i++)
		s += event[i];
	event += (char)(s % 256);
	String hash = "";
	for (int i = 0; i < event.length(); i++) {
		int c = (event[i] - (i == 0 ? 0 : event[i - 1]) + 256) % 256;
		int c1 = c / 16; int c2 = c % 16;
		char d1 = c1 < 10 ? '0' + c1 : 'a' + c1 - 10;
		char d2 = c2 < 10 ? '0' + c2 : 'a' + c2 - 10;
		hash += "%"; hash += d1; hash += d2;
	} 
	return hash;
}

String EventTaskClass::getHash(const int code, const String& date, const String& type, const String& value) {	
	String event = String(code);
	event += "\t" + date + "\t" + type + "\t" + value;
	int s = 0;
	for (int i = 0; i < event.length(); i++)
		s += event[i];
	event += (char)(s % 256);
	String hash = "";
	for (int i = 0; i < event.length(); i++) {
		int c = (event[i] - (i == 0 ? 0 : event[i - 1]) + 256) % 256;
		int c1 = c / 16; int c2 = c % 16;
		char d1 = c1 < 10 ? '0' + c1 : 'a' + c1 - 10;
		char d2 = c2 < 10 ? '0' + c2 : 'a' + c2 - 10;
		hash += "%"; hash += d1; hash += d2;
	} 
	return hash;
}*/

