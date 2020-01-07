#include "AxesClass.h"
#include "BrowserServer.h"
#include "SerialPort.h"
#include "Board.h"

AxesClass * Axes;

void AxesPointTaskClass::run() {
	Axes->doPoint(_weight);
}

void AxesClass::handle(float weight) {	
	if (fabs(weight) > *_levelDeterminer) {
		if (!_start) {
			_start = true;
			arrayClear();
			_onStartDeterminer();
			_event = false;
		}
		
		if (_stab > 0){
			if (_array.size() > MAX_ARRAY)
				return;
			if (_past == weight){
				_array.push_back(weight);
				if (_stab > (STABLE_MEASURE-3)){
					doPoint(weight); /* ѕосылаем данные дл€ клиентов */
				}
			}/*else{
				if(webSocket.availableForWriteAll())
					Axes->doPoint(weight);				/ * ѕосылаем данные дл€ клиентов * /
					//Board->add(new AxesPointTaskClass(weight));
			}*/		
			serialPort->pause();
		}else{			
			serialPort->resume();
		}
	}else if (fabs(weight) < *_levelDeterminer) {
		if (_start){
			_start = false;
			_onStopDeterminer();
			*_num_check += 1;
		}
	}
	if (_past == weight) {
		if (_stab > 0) {
			_stab--;
		}
	}else {
		_stab = STABLE_MEASURE;   
		_past = weight;
	}
};
//* ѕосылаем значение измерени€ */
void AxesClass::doPoint(float weight) {
	DynamicJsonBuffer jsonBuffer;
	JsonObject& json = jsonBuffer.createObject();
	json["cmd"] = "scd";
	json["d"] = weight;
	sendSocket(json);
};

/*void AxesClass::doArray(AsyncWebSocketClient * client) {
	DynamicJsonBuffer jsonBuffer;
	JsonObject& json = jsonBuffer.createObject();
	json["cmd"] = "sad";
	JsonArray& array = json.createNestedArray("a");	
	auto size = _array.size();	
	for (int i = 0; i < size; i++) {		
		array.add(_array[i]);
			//p["i"] = i;
		//p["d"] = _array[i];
	}
		String str = String();
		json.printTo(str);
		//webSocket.text(client->id(), str);
		//webSocket.textAll(str);
		client->text(str);
	//}
};*/

/* ѕосылаем команду старт определени€ осей */
void AxesClass::doStartDeterminer() {
	DynamicJsonBuffer jsonBuffer;
	JsonObject& json = jsonBuffer.createObject();
	json["cmd"] = "std";
	json["a"] = XK3118T1.getPoint();	/* точность после зап€той */
	sendSocket(json);	
};
/* ѕосылаем команду стоп определени€ осей */
void AxesClass::doEndDeterminer() {
	DynamicJsonBuffer jsonBuffer;
	JsonObject& json = jsonBuffer.createObject();
	json["cmd"] = "ecd";
	sendSocket(json);	
}
;

void AxesClass::sendSocket(JsonObject& json) {
	String str = String();
	json.printTo(str);	
	webSocket.textAll(str);
};

/*vector<double> AxesClass::computeJumps(vector<double>& levels) {
	vector<double> jumps(levels.size() - 1);
	for (int i = 0; i < jumps.size(); i++)
		jumps[i] = abs(levels[i + 1] - levels[i]);
	return jumps;
}*/

/*double AxesClass::computeDeviation(AxesWeighingType axesWeighingType, vector<double>& levels) {
	vector<double> jumps = computeJumps(levels);
	double maxDeviation = numeric_limits<double>::min();
	switch (axesWeighingType) {
	case DRIVE_ONTO_AND_BACK:
		for (int i = 0; i < jumps.size() / 2; i++) {
			double deviation = abs(jumps[i] - jumps[jumps.size() - i - 1]);
			if (deviation > maxDeviation)
				maxDeviation = deviation;
		}
		return maxDeviation;
	case DRIVE_THROUGH:
		for (int i = 0; i < jumps.size() / 2; i++) {
			double deviation = abs(jumps[i] - jumps[jumps.size() / 2 + i]);
			if (deviation > maxDeviation)
				maxDeviation = deviation;
		}
		return maxDeviation;
	}
}*/

/*vector<double> AxesClass::computeNormedLevels(AxesWeighingType axesWeighingType, vector<int> levelBegins, vector<int> levelEnds, vector<double> levels) {
	vector<double> normedLevels(levels.size());
	switch (axesWeighingType) {
	case DRIVE_ONTO_AND_BACK:
		for (int i = 1; i <= levels.size() / 2; i++)
			normedLevels[i] = normedLevels[levels.size() - i - 1] = ((levelEnds[i] - levelBegins[i]) * levels[i] + (levelEnds[levels.size() - i - 1] - levelBegins[levels.size() - i - 1]) * levels[levels.size() - i - 1]) / (levelEnds[i] - levelBegins[i] + levelEnds[levels.size() - i - 1] - levelBegins[levels.size() - i - 1]);
		return normedLevels;
	case DRIVE_THROUGH:
		vector<double> jumps = computeJumps(levels);
		normedLevels[normedLevels.size() / 2] = levels[levels.size() / 2];
		for (int i = 1; i < levels.size() / 2; i++) {
			normedLevels[i] = normedLevels[i - 1] + ((levelEnds[i] - levelBegins[i]) * jumps[i - 1] + (levelEnds[levels.size() / 2 + i] - levelBegins[levels.size() / 2 + i]) * jumps[levels.size() / 2 + i - 1]) / (levelEnds[i] - levelBegins[i] + levelEnds[levels.size() / 2 + i] - levelBegins[levels.size() / 2 + i]);
			normedLevels[levels.size() / 2 + i] = normedLevels[normedLevels.size() / 2] - normedLevels[i];
		}
		return normedLevels;
	}
}*/

/** ќпределить оси по показани€м весов
	 * @param weights показани€ весов
	 * @return веса осей
	 * @exception AxesException не удалось определить оси
	 */
/*AxesWeighing AxesClass::determineAxes(vector<double>& weights) {
	vector<double> sortedWeights = weights;
	sort(sortedWeights.begin(), sortedWeights.end());
	double maxDeviation = sortedWeights[weights.size() - 1] - sortedWeights[0];
	double minDeviation = maxDeviation;
	for (int i = 1; i < weights.size(); i++) {
		double deviation = abs(sortedWeights[i] - sortedWeights[i - 1]);
		if (deviation > 0 && deviation < minDeviation)
			minDeviation = deviation;
	}
	double minResidual = numeric_limits<double>::max();
	AxesWeighing axesWeighing;
	bool success = false;
	for (double deviation = minDeviation; deviation < maxDeviation; deviation *= 1.1)
		for (int stabilizationTime = weights.size(); stabilizationTime > weights.size() / 20; stabilizationTime /= 1.1) {
			vector<int> levelBegins; levelBegins.push_back(0);
			vector<int> levelEnds; levelEnds.push_back(0);
			vector<double> levels; levels.push_back(0.0);
			for (int levelBegin = 0, levelEnd = 1; levelEnd < weights.size(); levelEnd++) {
				while (levelEnd < weights.size() && abs(weights[levelEnd] - weights[levelBegin]) < deviation)
					levelEnd++;
				if (levelEnd - levelBegin >= stabilizationTime) {
					vector<double> levelWeights(levelEnd - levelBegin);
					copy(weights.begin() + levelBegin, weights.begin() + levelEnd, levelWeights.begin());
					sort(levelWeights.begin(), levelWeights.end());
					levelBegins.push_back(levelBegin);
					levelEnds.push_back(levelEnd);
					levels.push_back(levelWeights[levelWeights.size() / 2]);
					levelBegin = levelEnd;
				}
				else
					levelEnd = ++levelBegin;
			}
			levelBegins.push_back(weights.size());
			levelEnds.push_back(weights.size());
			levels.push_back(0.0);
			if (levels.size() % 2 == 0)
				continue;
			for (int axesWeighingType = 0; axesWeighingType < 2; axesWeighingType++) {
				if (computeDeviation(static_cast<AxesWeighingType>(axesWeighingType), levels) >= deviation)
					continue;
				vector<double> normedLevels = computeNormedLevels(static_cast<AxesWeighingType>(axesWeighingType), levelBegins, levelEnds, levels);
				vector<double> approximatedWeights(weights.size());
				for (int i = 1; i < levels.size(); i++) {
					double slope = (normedLevels[i] - normedLevels[i - 1]) / (levelBegins[i] - levelEnds[i - 1]);
					double intercept = (normedLevels[i - 1] * levelBegins[i] - normedLevels[i] * levelEnds[i - 1]) / (levelBegins[i] - levelEnds[i - 1]);
					for (int j = levelEnds[i - 1]; j < levelBegins[i]; j++)
						approximatedWeights[j] = slope * j + intercept;
					for (int j = levelBegins[i]; j < levelEnds[i]; j++)
						approximatedWeights[j] = normedLevels[i];
				}
				double residual = 0.0;
				for (int i = 0; i < weights.size(); i++)
					residual += abs(weights[i] - approximatedWeights[i]);
				residual /= weights.size();
				if (residual < minResidual) {
					vector<double> axes(levels.size() / 2);
					for (int i = 0; i < axes.size(); i++)
						axes[i] = normedLevels[i + 1] - normedLevels[i];
					axesWeighing.set(weights, approximatedWeights, static_cast<AxesWeighingType>(axesWeighingType), axes, deviation, stabilizationTime, residual);
					success = true;
					minResidual = residual;
				}
			}
		}
	if (!success)
		throw AxesException();
	return axesWeighing;
}*/