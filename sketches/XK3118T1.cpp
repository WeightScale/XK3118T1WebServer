#include "XK3118T1.h"

KeliXK3118T1Class XK3118T1;

void KeliXK3118T1Class::handlePort() {
	/*if (serialPort->available()) {
		if (serialPort->read() == _sync_byte) {
			int i = 1;
			_w = "";
			_point = 0;
			while (serialPort->available()) {
				int b = serialPort->read();
				if (i >= _start && i <= _end) {							
					_w = _reverse ? char(b) + _w : _w + char(b);
					if (b == '.') {
						_point = _reverse ? i - _start : _end - i;	
					}
				}
				//if (i == _end) {
				if ((char)b == '\n') {
					_weight = _w.toFloat();
					//serialPort->println(_weight);
					//serialPort->printf("[XK3118T1] convert data: %f\n", _weight);
					//DEBUG_BOARD("[XK3118T1] convert data: %f\n", _weight);
					_time = millis() + serialPort->getTime();
					return;
				}
				i++;
			}	
		}				
	}*/
	
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
