#include "Battery.h"
#include "BrowserServer.h"
#include "Board.h"

BatteryClass::BatteryClass(unsigned int *min, unsigned int *max) : Task(20000) {
	/* 20 Обновляем заряд батареи */
	onRun(std::bind(&BatteryClass::fetchCharge, this));
	_max = max;
	_min = min;	
	//fetchCharge();
}

unsigned int BatteryClass::fetchCharge() {
#ifndef DEBUG_BATTERY
	_charge = _get_adc(1);
	_charge = constrain(_charge, *_min, *_max);
	_charge = map(_charge, *_min, *_max, 0, 100);
	if (_charge <= 5){
		if(_onEventDischarged)
			_onEventDischarged(_charge);	
	}	
#else
	_charge = 51;
#endif // !DEBUG_BATTERY	
	if (_onEvent)
		_onEvent(_charge);
	return _charge;
}

unsigned int BatteryClass::_get_adc(byte times) {
	unsigned long sum = 0;
#ifdef DEBUG_BATTERY
	for (byte i = 0; i < times; i++) {
		sum += random(ADC);
	}	
#else
	for (byte i = 0; i < times; i++) {
		sum += analogRead(V_BAT);
	}
#endif // DEBUG_BATTERY	
	return times == 0 ? sum : sum / times;	
}

size_t BatteryClass::doInfo(JsonObject& json) {
	json["id_min"] = CALCULATE_VIN(*_min);
	json["id_max"] = CALCULATE_VIN(*_max);
	json["id_cvl"] = CALCULATE_VIN(_get_adc(1));
	return json.measureLength();	
}

size_t BatteryClass::doData(JsonObject& json) {
	json["cmd"] = "btr";
	json["c"] = _charge;
	return json.measureLength();
};
