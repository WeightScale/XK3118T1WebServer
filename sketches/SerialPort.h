#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "Task.h"
#include "Config.h"

#define PORT_FILE		"/port.json"
#define PORT_TERMINAL_JSON	"tr_id"
#define PORT_SPEED_JSON		"sp_id"
#define PORT_ACCURACY_JSON	"ac_id"
#define PORT_USER_JSON		"us_id"
#define PORT_PASS_JSON		"ps_id"

//#define STABLE_NUM_MAX		100
#define STABLE_DELTA_STEP	10

#define szCR_LF					"\x0D\x0A"
#define CR						0xd
#define LF						0xa

// setport.html
const char setport_html[] PROGMEM = R"(<!DOCTYPE html><html lang="en"><head> <meta charset="UTF-8"> <meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1'/> <meta http-equiv="Cache-Control" content="no-cache, no-store, must-revalidate"/> <meta http-equiv="Pragma" content="no-cache"/> <title>Настройки данных</title> <link rel="stylesheet" type="text/css" href="global.css"> <style>td{font-size: 5vw;}table{width: 100%;}input{width: 100%; text-align: right;}input[type='submit']{width: auto; box-shadow: 1px 4px 8px 1px #585858;}input[type=checkbox]{height: 4vw;}select{width: 100%; text-align-last: right; border: 1px solid #ccc;}@media (min-width: 640px){td{font-size: 32px;}input[type=checkbox]{height: 25px;}}</style> <script>var d=document; function _(e){return d.getElementById(e);}function _gs(){var http_request=new XMLHttpRequest(); http_request.overrideMimeType('application/json'); http_request.onreadystatechange=function(){if (http_request.readyState===4){var json=JSON.parse(http_request.responseText); for (entry in json){if (_(entry) !==null) _(entry).value=json[entry];}d.body.style.visibility='visible';}}; http_request.open("GET", "/port.json", true); http_request.send(null);}window.onload=function(){_gs();}; function saveValue(f){var fd=new FormData(_(f)); var r=new XMLHttpRequest(); r.onreadystatechange=function(){if (r.status===200) window.open('/', '_self'); else if (r.status===204){if (confirm('Настройки не изменились. Выйти?')){window.open('/', '_self');}}}; r.onerror=function(){alert("Ошибка: " + this.src);}; r.open('POST', 'setport.html', true); r.send(fd);}; </script></head><body style="visibility: hidden"><a href='/settings.html'><img src='und.png' alt="&lt;" class='btn btn--s btn--blue'></a>&nbsp;&nbsp;<strong>НАСТРОЙКА ПОРТА</strong><hr><fieldset id="form_max"> <legend>Общии настройки</legend> <form id="form_id" method='POST'> <table> <tr> <td>Скорость порта</td><td> <select id="sp_id" name="spd" title="Выбор скорости COM порта" style="width: 100%"> <option name="600" value="600"> 600 </option> <option name="1200" value="1200"> 1200 </option> <option name="2400" value="2400"> 2400 </option> <option name="4800" value="4800"> 4800 </option> <option name="9600" value="9600"> 9600 </option> <option name="19200" value="19200"> 19200 </option> <option name="38400" value="38400"> 38400 </option> <option name="115200" value="115200"> 115200 </option> </select> </td></tr><tr> <td>Время</td><td> <input title='Время ожидания данных милисек' type='number' step='any' min='100' max='10000' id='tm_id' name='tme'> </td></tr><tr> <td>Старт измерения</td><td> <input title='Начало процесса измерения осей' type='number' step='any' max='1000' id='sdt_id' name='sdt'> </td></tr></table> <h2><a href='javascript:saveValue("form_id");'>сохранить и выйти</a></h2> </form></fieldset><br><fieldset> <details> <summary>Авторизация для терминала</summary> <form method='POST'> <table> <tr> <td>ИМЯ:</td><td> <input id='us_id' name='user' placeholder='имя админ'> </td></tr><tr> <td>ПАРОЛЬ:</td><td> <input type='password' id='ps_id' name='pass' placeholder='пароль админ'> </td></tr><tr> <td></td><td> <input type='submit' value='СОХРАНИТЬ'/> </td></tr></table> </form> </details></fieldset><hr><footer align="center">2018 © Powered by www.scale.in.ua</footer></body></html>)";


class SerialPortClass : public HardwareSerial, public AsyncWebHandler, public Task {
protected:
	serial_port_t * _value;
	bool _authenticated;
		
	void _saveValuePortHttp(AsyncWebServerRequest * request);
	
	//bool loadPortValue();		

public:			
	SerialPortClass(int port, serial_port_t * value);
	virtual ~SerialPortClass(){};
	virtual bool canHandle(AsyncWebServerRequest *request) override final;
	virtual void handleRequest(AsyncWebServerRequest *request) override final;
	virtual bool isRequestHandlerTrivial() override final {return false;};
	void takeWeight();
	void handleValue(AsyncWebServerRequest * request);
	//int getAccuracy(){return _value->accuracy;};
	int getTime() {return _value->time;};
};

extern SerialPortClass * serialPort;
