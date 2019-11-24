#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>

extern "C" uint32_t _SPIFFS_start;
extern "C" uint32_t _SPIFFS_end;

const char successResponse[] PROGMEM = R"(<meta http-equiv='refresh' content='15;URL=/'>Обновление успешно! Не выключайте пока не перегрузится...)";

const char serverIndex[] PROGMEM = R"(<!DOCTYPE html><html><head> <meta charset='UTF-8'> <meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1.0, user-scalable=no'/> <link rel='stylesheet' type='text/css' href='global.css'> <script>let d=document; function _(el){return d.getElementById(el);}function upload(){var fl=_('id_file').files[0]; var fd=new FormData(); fd.append('update', fl); var r=new XMLHttpRequest(); r.upload.addEventListener('progress', progressHandler, false); r.addEventListener('load', cHandler, false); r.addEventListener('error', eHandler, false); r.addEventListener('abort', eHandler, false); r.open('POST', '/update'); r.send(fd); _('id_p').hidden=false}function progressHandler(e){let p=(e.loaded / e.total) * 100; _('id_prg').value=Math.round(p); _('id_st').innerHTML=Math.round(p) + '% загружено... подождите';}function cHandler(e){_('id_st').innerHTML=e.target.responseText; _('id_prg').hidden=true;}function eHandler(e){_('id_st').innerHTML=e.toString();}</script></head><body><form method='POST' action='' enctype='multipart/form-data'> <input id='id_file' style='width: auto' type='file' name='update'> <hr> <input type='button' value='Обновить' onclick='upload()'></form><div id="id_p" hidden> <br><progress style="width: 100%" id='id_prg' value='0' max='100'></progress> <h3 id='id_st'></h3></div></body></html>)";

//const char serverIndex[] PROGMEM = R"(<!DOCTYPE html><html><head> <meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1.0, user-scalable=no'/> <link rel='stylesheet' type='text/css' href='global.css'> </head><body><form method='POST' action='' enctype='multipart/form-data'> <input style="width: auto" type='file' name='update'> <hr> <input type='submit' value='Update'> </form></body></html>)";

class UpdaterLocalClass : public AsyncWebHandler{
public:
	UpdaterLocalClass();
	UpdaterLocalClass(const String& username = String(), const String& password = String());	
	~UpdaterLocalClass() {};		
	void setAuthenticated(bool a){_authenticated = a;};
	bool getAuthenticated(){return _authenticated;};
	virtual bool canHandle(AsyncWebServerRequest *request) override final;
	virtual void handleRequest(AsyncWebServerRequest *request) override final;
	virtual void handleUpload(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) override final;	
	virtual bool isRequestHandlerTrivial() override final {return false;}
	

protected:
	void setUpdaterError();
private:				
	String _username;
	String _password;
	bool _authenticated = false;
	int ClientId;
	//bool _process = false;
};

extern UpdaterLocalClass UpdaterLocal;