#pragma once
#include <Arduino.h>
#include "Task.h"
#include <ESPAsyncWebServer.h>
#include "Config.h"
#include <vector>
#include "ArduinoJson.h"
#include "Event.h"

using namespace std;

#define STABLE_MEASURE		20
#define MAX_ARRAY			1000

/*class AxesArrayTaskClass : public Task {
protected:
	AsyncWebSocketClient * _client;
public:
	AxesArrayTaskClass(AsyncWebSocketClient * client)
		: Task(0, true)
		, _client(client) {};
	~AxesArrayTaskClass() {};
	virtual void run() override final;
};*/

class AxesPointTaskClass : public Task {
protected:
	float _weight;
public:
	AxesPointTaskClass(float weight) : Task(5, true), _weight(weight) {};
	~AxesPointTaskClass() {};
	virtual void run() override final;
};

class AxesEventClass : public EventTaskClass{
private:
	
public:
	AxesEventClass(String value): EventTaskClass(EVENT_WEIGHT_AXES, value) {};
	~AxesEventClass() {};
};

class AxesCommandTaskClass : public Task{
protected:
	AsyncWebSocketClient * _client;
	String _msg;
public:
	AxesCommandTaskClass(AsyncWebSocketClient * client, String &msg)	: Task(100, true), _client(client),_msg(msg) {};
	~AxesCommandTaskClass() {};
	void run() {
		_client->text(_msg);
	};	
};

class AxesClass {
	typedef void(*_Func)(void);
private:
	String _check;
	//AsyncWebSocket &_socket;		/* Указатель на сокет */
	unsigned int *_num_check;	/* Номер текущего чека */
	bool _start = false;		/* Флаг начала и конца апроксимации */
	float _past;				/* Значение предыдущего веса для определения стабилизации */
	unsigned int _stab;			/* Индикатор стабильного веса при стабилизации идет на уменшение */
	bool _event;				/* Если  true то текущее событие было отправлено */
	_Func _onStartDeterminer;	
	_Func _onStopDeterminer;
#ifdef DEBUG_SERIAL
public:
	std::vector<float> _array;
#else									
public:
	std::vector<float> _array;
#endif // DEBUG_SERIAL
	float * _levelDeterminer; 
public:
	AxesClass( /*AsyncWebSocket &socket,*/ unsigned int *num)	: /*_socket(socket),*/ _num_check(num) {};
	~AxesClass() {};
	void begin(float* level) {
		_levelDeterminer = level;
	};
	void handle(float weight);
	float* levelDeterminer() {return _levelDeterminer;};
	void levelDeterminer(float *level) {_levelDeterminer = level; };
	void onStartDeterminer(_Func callback) {_onStartDeterminer = callback; };
	void onStopDeterminer(_Func callback) {_onStopDeterminer = callback; };
	void doPoint(float weight);
	//void doArray(AsyncWebSocketClient* client);
	void doStartDeterminer();
	void doEndDeterminer();
	void sendSocket(JsonObject& json);
	//std::vector<double> *array() {return &_array;};
	bool start() {return _start;};
	unsigned int stab() {return _stab;};
	bool event() {return _event;};
	void event(bool ev) {_event=ev;};
	unsigned int *num() {return _num_check;};
	void check(const String &check) {_check = check;};
	String &check() {return _check;};
};

/** Не удалось определить оси */
/*class AxesException {
};*/

/** Тип взвешивания осей */
/*enum AxesWeighingType {
	/ ** Заезд и съезд задом * /
	DRIVE_ONTO_AND_BACK = 0,
	/ ** Проезд по весам * /
	DRIVE_THROUGH = 1
};*/

/** Результат взвешивания осей */
/*class AxesWeighing {
	/ ** Реальные веса * /
	vector<double> _weights;
	/ ** Приближённые веса * /
	vector<double> _approximatedWeights;
	/ ** Тип взвешивания осей * /
	AxesWeighingType _axesWeighingType;
	/ ** Веса осей * /
	vector<double> _axes;
	/ ** Отклонение (для тестирования) * /
	double _deviation;
	/ ** Время стабилизации (для тестирования) * /
	int _stabilizationTime;
	/ ** Невязка (для тестирования) * /
	double _residual;

public:
	AxesWeighing() {};
	~AxesWeighing()	{};

	void set(vector<double> weights, vector<double> approximatedWeights, AxesWeighingType axesWeighingType, vector<double> axes, double deviation, int stabilizationTime, double residual) {
		_weights = weights;
		_approximatedWeights = approximatedWeights;
		_axesWeighingType = axesWeighingType;
		_axes = axes;
		_deviation = deviation;
		_stabilizationTime = stabilizationTime;
		_residual = residual;
	}

	String toString() {
		stringstream stringstream_;
		stringstream_ << "Тип взвешивания осей = " << (_axesWeighingType == 0 ? "Заезд передом и съезд задом" : "Проезд") << endl;
		stringstream_ << "Общий вес = " << totalWeight() << endl;
		for (int i = 0; i < _axes.size(); i++)
			stringstream_ << "* Ось № " << i + 1 << " = " << _axes[i] << endl;
		stringstream_ << "(Отклонение = " << _deviation << endl;
		stringstream_ << "(Время стабилизации = " << _stabilizationTime << ")" << endl;
		stringstream_ << "(Невязка = " << _residual << ")" << endl;
		return stringstream_.str();
	}

	/ ** Общий стабильный вес * /
	double totalWeight() {
		double sum = 0.0;
		for (int i = 0; i < _axes.size(); i++)
			sum += _axes[i];
		return sum;
	}

	vector<double>& getWeights() {
		return _weights;
	}

	vector<double>& getApproximatedWeights() {
		return _approximatedWeights;
	}

	vector<double>& getAxes() {
		return _axes;
	}
};*/

extern AxesClass *Axes;