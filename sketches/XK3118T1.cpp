#include "XK3118T1.h"

KeliXK3118T1Class XK3118T1;

void KeliXK3118T1Class::handlePort() {
	if (serialPort->available()) {
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
				if (i == _end) {
					_weight = _w.toFloat();
					_time = millis() + serialPort->getTime();
					return;
				}
				i++;
			}	
		}				
	}							
};
