#pragma once
#include "Terminal.h"

class KeliXK3118T1Class : public TerminalClass {	
	/* =5123.45(kg)\xd\xa */
private:
	bool _reverse = false;
	int _start = 1, _end = 7;
	int _sync_byte = '=';
		
public:
	KeliXK3118T1Class(){};
	~KeliXK3118T1Class(){};
	void handlePort();	
	
	//bool saveValueHttp(AsyncWebServerRequest * request){};
	//bool downloadValue(int inx){};
};

extern KeliXK3118T1Class XK3118T1;