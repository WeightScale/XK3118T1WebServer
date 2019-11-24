#include "Board.h"

#ifdef ESP8266_USE_GDB_STUB
	#include <GDBStub.h>
	extern "C" int gdbstub_init();
	extern "C" int gdbstub_do_break();
#endif

void setup(){
#ifdef ESP8266_USE_GDB_STUB
	Serial.begin(921600);
    gdbstub_init();
    gdbstub_do_break();
#else
	//Serial.begin(9600);
	//Serial.println("Start");
#endif
	Board = new BoardClass();
	Board->init();
	server.begin();
	//DEBUG_BOARD("START...");
	serialPort->printf("START...");
}

void loop(){
	Board->handle();
#ifdef MULTI_POINTS_CONNECT
	Board->wifi()->connect();
	delay(1);
#endif // MULTI_POINTS_CONNECT
	//serialPort->println("Start");
	//DEBUG_BOARD("START...");
	/*if(serialPort->available()) {
		if (serialPort->read() == '=') {
			String _w = "=";
			while (serialPort->available()) {
				int b = serialPort->read();
				_w = _w + char(b);
				if ((char)b == ')') {
					serialPort->println(_w);
					return;
				}
			}	
		}
		
	}
	*/
	/*String str = String();
	while(serialPort->available()) {
		int b = serialPort->read();
		str = str + (char)b;
		//serialPort->write(b);
		if ((char)b == '\n') {
			serialPort->print(str);
			str = "";
		}
	}
	uint64_t timeOld = millis();
	while ((millis() < (timeOld + 2))) {
		
		while (serialPort->available()) {
			int b = serialPort->read();
			str = str + (char)b;
			//serialPort->write(b);
			if((char)b == '\n') {
				serialPort->print(str);
				str = "";
			}
			timeOld = millis();
		}		
	}*/
}
