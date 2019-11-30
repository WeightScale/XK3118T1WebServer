
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

class TerminalClass {
	//typedef void(*TerminalEventFunc)(float);
	typedef std::function<void(float)> TerminalEventFunc;
	protected:				
		String _w;		
		float _weight;
		bool _stableWeight;
		t_save_value _saveWeight = { 0 };
		long int _time;
		int _point = 0;				//где находится точка
		TerminalEventFunc _onEvent;
	public :	
		TerminalClass() {};
		virtual ~TerminalClass() {};			
		virtual void handlePort(){};
		virtual bool saveValueHttp(AsyncWebServerRequest * request){return false;};
		virtual bool downloadValue(int){return false;};
		virtual size_t doData(JsonObject& json );
		//virtual void formatValue(char* string){dtostrf(_weight, 6-serialPort->getAccuracy(), serialPort->getAccuracy(), string);};
		virtual void formatValue(char* string){dtostrf(_weight, 6 - _point, _point, string); };
		virtual String timeCheck(char * b){return _time < millis()? String("no data"):String(b);}; // проверка данных на время если нет новых данных в течении 4 сек			
		float getWeight(){return _weight;};
#ifndef SCALES_AXES
		void detectStable();	  
#endif // !SCALES_AXES
		bool isSave() {return _saveWeight.isSave;};
		void setIsSave(bool s) {_saveWeight.isSave = s;};
		float get_save_value() {
			return _saveWeight.value;
		};
		void onEvent(TerminalEventFunc callback){_onEvent = callback; };
		int getPoint() {return _point;};
		void setStableNum(int stab) {_saveWeight.stabNum = stab;};
};

//extern TerminalClass *Terminal;

#endif

