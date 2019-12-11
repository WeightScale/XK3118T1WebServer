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
#define MAX_ARRAY			2000

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

class AxesClass {
private:
	AsyncWebSocket* _socket;
	unsigned int *_num_check;
	bool _start = false;
	float _past;
	unsigned int _stab;
	bool _event;			/* ����  true �� ������� ������� ���� ���������� */
#ifdef DEBUG_SERIAL
public:
	std::vector<double> _array;
#else									
public:
	std::vector<double> _array;
#endif // DEBUG_SERIAL
	float * _levelDeterminer; 
public:
	AxesClass(AsyncWebSocket* socket, unsigned int *num)	: _socket(socket), _num_check(num) {};
	~AxesClass() {};
	void begin(float* level) {
		_levelDeterminer = level;
	};
	void handle(float weight);
	float* levelDeterminer() {return _levelDeterminer;};
	void levelDeterminer(float *level) {_levelDeterminer = level; };
	void doPoint(float weight);
	//void doArray(AsyncWebSocketClient* client);
	void doStartDeterminer();
	void doEndDeterminer();
	void sendSocket(JsonObject& json);
	bool start() {return _start;};
	unsigned int stab() {return _stab;};
	bool event() {return _event;};
	void event(bool ev) {_event=ev;};
	unsigned int *num() {return _num_check;};
};

/** �� ������� ���������� ��� */
/*class AxesException {
};*/

/** ��� ����������� ���� */
/*enum AxesWeighingType {
	/ ** ����� � ����� ����� * /
	DRIVE_ONTO_AND_BACK = 0,
	/ ** ������ �� ����� * /
	DRIVE_THROUGH = 1
};*/

/** ��������� ����������� ���� */
/*class AxesWeighing {
	/ ** �������� ���� * /
	vector<double> _weights;
	/ ** ����������� ���� * /
	vector<double> _approximatedWeights;
	/ ** ��� ����������� ���� * /
	AxesWeighingType _axesWeighingType;
	/ ** ���� ���� * /
	vector<double> _axes;
	/ ** ���������� (��� ������������) * /
	double _deviation;
	/ ** ����� ������������ (��� ������������) * /
	int _stabilizationTime;
	/ ** ������� (��� ������������) * /
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
		stringstream_ << "��� ����������� ���� = " << (_axesWeighingType == 0 ? "����� ������� � ����� �����" : "������") << endl;
		stringstream_ << "����� ��� = " << totalWeight() << endl;
		for (int i = 0; i < _axes.size(); i++)
			stringstream_ << "* ��� � " << i + 1 << " = " << _axes[i] << endl;
		stringstream_ << "(���������� = " << _deviation << endl;
		stringstream_ << "(����� ������������ = " << _stabilizationTime << ")" << endl;
		stringstream_ << "(������� = " << _residual << ")" << endl;
		return stringstream_.str();
	}

	/ ** ����� ���������� ��� * /
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