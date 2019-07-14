
#ifndef _TERMINAL_h
#define _TERMINAL_h
#include <Arduino.h>
#include "BrowserServer.h"
#include "SerialPort.h"

#define TERMINAL_FILE "/terminal.json"
#define TERMINAL_TERMINAL_JSON "trm"
#define TIME_CHECK 4000		//время для проверки на новые данные

typedef struct {
	bool isSave;
	int stabNum;
	float value;
	unsigned long time;
}t_save_value;

class TerminalClass  {
	protected:				
		String _w;		
		float _weight;
		bool _stableWeight;
		t_save_value _saveWeight = { 0 };
		long int _time;
		int _point = 0;				//где находится точка
	public:	
		TerminalClass(){};
		//~TerminalClass();			
		virtual void handlePort(){};
		virtual bool saveValueHttp(AsyncWebServerRequest * request){return false;};
		virtual bool downloadValue(int){return false;};
		virtual size_t doData(JsonObject& json );
		virtual void formatValue(char* string){dtostrf(_weight, 6-serialPort->getAccuracy(), serialPort->getAccuracy(), string);};
		virtual String timeCheck(char * b){return _time < millis()? String("no data"):String(b);}; // проверка данных на время если нет новых данных в течении 4 сек			
		float getWeight(){return _weight;};
		void detectStable();
		bool isSave() {return _saveWeight.isSave;};
		void setIsSave(bool s) {_saveWeight.isSave = s;};
		float get_save_value() {
			return _saveWeight.value;
		};
};

//extern TerminalClass *Terminal;

#endif

