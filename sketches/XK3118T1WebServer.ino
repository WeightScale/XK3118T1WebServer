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
#ifdef DEBUG_SERIAL
	int ii = 10;
	for (int i=0; i < ii; i++){
		Axes._array.push_back(i*100);		
	}
#endif // DEBUG_SERIAL

}

void loop(){
	Board->handle();
#ifdef MULTI_POINTS_CONNECT
	Board->wifi()->connect();
	delay(1);
#endif // MULTI_POINTS_CONNECT
	//Axes.handle(100);
}
