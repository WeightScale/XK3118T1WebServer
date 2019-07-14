#include "Terminal.h"
#include <string>
#include <cstdlib>
#include <algorithm>
#include <Arduino.h>



size_t TerminalClass::doData(JsonObject& json ){	
	char buff[10];
	formatValue(buff);
	json["w"]= timeCheck(buff);
	json["s"]= _saveWeight.stabNum;
	return json.measureLength();
	
}

void TerminalClass::detectStable() {	
	if (_saveWeight.value != _weight) {
		_saveWeight.stabNum = STABLE_NUM_MAX;
		_stableWeight = false;
		_saveWeight.value = _weight;
		return;
	}
	
	if (_saveWeight.stabNum) {
		_saveWeight.stabNum--;
		return;
	}	
	if (_stableWeight) {
		return;
	}
	_stableWeight = true;
	if (millis() < _saveWeight.time)
		return;
	_saveWeight.isSave = true;
	_saveWeight.time = millis() + 10000;
}