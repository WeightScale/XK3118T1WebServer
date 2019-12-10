#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include "Task.h"
#include <ArduinoJson.h>

const char check_html[] PROGMEM = R"(<!DOCTYPE html><html lang="ru"><head> <meta charset='UTF-8'> <meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1.0, user-scalable=no'/> <link rel='stylesheet' type='text/css' href='global.css'> <link rel='shortcut icon' href='favicon.png' type='image/png'> <title>WEB SCALES</title> <style>input:focus{background: #FA6; outline: none;}table{width: 100%;}td{width: 50%; text-align: center;}input[type=button]{width: auto; text-align: right; box-shadow: 1px 4px 8px 1px #585858;}</style> <script>let d=document, w=null; function _(el){return d.getElementById(el);}function ScalesSocket(h, p,fo,fm, fe){let host=h, protocol=p, tw, ts, ws; this.snd=function(c){ws.send(c);}; this.openSocket=function(){ws=new WebSocket(host, protocol); ws.onopen=function(){fo();}; ws.onclose=function(){fe();}; ws.onerror=function(e){ws.close(); fe(e);}; ws.onmessage=function(e){fm(JSON.parse(e.data));}};}function parseCommand(e){if (e.hasOwnProperty('cmd')){switch (e.cmd){case 'update': if (e.code===200){_('id_msg').innerHTML="Есть новое обновление"; var table=d.createElement('table'); for (i in e.files){var row=d.createElement('tr'), a=d.createElement('a'); a.innerHTML=e.files[i]; a.href='javascript:update("' + e.files[i] + '");'; var td=d.createElement('td'); td.appendChild(a); row.appendChild(td); table.appendChild(row);}_('id_msg').appendChild(table);}else if (e.code===304 || e.code===1) _('id_msg').innerHTML='Нет обновлений'; else if (e.code===2){var rec=confirm('Обновление успешно. Перегрузить?'); if (rec){var r=new XMLHttpRequest(); r.onload=function(){_('id_st').innerHTML=r.responseText; _('id_prg').hidden=true;}; r.open('GET', '/rc', true); r.send(null);}else{_('id_prg').hidden=true; check();}}else if(e.code===0) _('id_msg').innerHTML='Ошибка при обновлении'; break; case 'status': break; case 'check': if (e.code===200) _('id_msg').innerHTML='Проверяем обновления ...'; else _('id_msg').innerHTML='Нет подключения интернета'; break; case 'prog': progress(e); break;}}}function progress(e){let p=(e.loaded / e.total) * 100; _('id_prg').value=Math.round(p); _('id_st').innerHTML=Math.round(p) + '% загружено... подождите';}function update(f){var rec=confirm(f+': Обновится сейчас?'); if (rec){w.snd('{"cmd":"update","file":"'+ f +'"}'); _('id_msg').innerHTML=f; _('id_p').hidden=false;}}function check(){_('id_p').hidden=true; w.snd('{"cmd":"check"}');}window.onload=function(){w=new ScalesSocket('ws://' + d.location.host + '/ws', ['scales'],function(){check();}, parseCommand, function(e){_('id_msg').innerHTML=e.toString();}); w.openSocket();}; </script></head><body><table> <tr> <td> <h3 id='id_msg'></h3> </td></tr><tr> <td> <div id="id_p" hidden> <progress style="width: 100%" id='id_prg' value='0' max='100'></progress> <h3 id='id_st'></h3> </div></td></tr></table><table> <tr> <td> <input type='button' value='Проверить обновления' onclick='check()'> </td></tr></table></body></html>)";

class UpdaterHttpClass : public AsyncWebHandler {
private:	
	String _username;
	String _password;
protected:
	void setUpdaterError();	
public:
	UpdaterHttpClass();
	UpdaterHttpClass(const String& username = String(), const String& password = String());	
	~UpdaterHttpClass() {};	
	virtual bool canHandle(AsyncWebServerRequest *request) override final;
	virtual void handleRequest(AsyncWebServerRequest *request) override final;
	virtual bool isRequestHandlerTrivial() override final {return false;};
};

enum FileTypeEnum{
	UNKNOW_TYPE,	         
	SPIFFS_TYPE,
	INO_TYPE
};

class CheckTaskClass : public Task {
protected:
	AsyncWebSocketClient *_client;
public:
	CheckTaskClass(AsyncWebSocketClient *client) : Task(100, true), _client(client){};
	~CheckTaskClass() {};
	virtual void run() override final;
};

class UpdateTaskClass : public Task {
protected:
	AsyncWebSocketClient *_client;
	String _file;
	FileTypeEnum _type = UNKNOW_TYPE;
	
public:
	UpdateTaskClass(JsonObject& json, AsyncWebSocketClient *client) : Task(100, true) {
		_client = client;
		_file = json["file"].as<String>();
		if (_file.indexOf(".spiffs") != -1) {
			_type = SPIFFS_TYPE;	
		}else if (_file.indexOf(".ino") != -1) {
			_type = INO_TYPE;
		}else {
			_type = UNKNOW_TYPE;
		}	
	};
	~UpdateTaskClass() {};
	virtual void run() override final;
};

extern UpdaterHttpClass * UpdaterHttp;