#include "XK3118T1.h"
#include "SerialPort.h"

KeliXK3118T1Class XK3118T1;

void KeliXK3118T1Class::handlePort() {		
	while (serialPort->available()){
		int b = serialPort->read();			
		if (_byte_num > 0){
			if (_byte_num >= _start && _byte_num <= _end) {							
				_w = _reverse ? char(b) + _w : _w + char(b);
				if (b == '.') {
					_point = _reverse ? _byte_num - _start : _end - _byte_num;	
				}
			}			
			if (_byte_num == _end) {
				_weight = _w.toFloat();
				_time = millis() + serialPort->getTime();
				_byte_num = 0;
				_onEvent(_weight);
			}else
				_byte_num++;
		}else{
			if (b == _sync_byte){
				_byte_num = 1;
				_w = "";
				//_point = 0;
			}			
		}	
		yield();
	}
};
