﻿#pragma once
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "Config.h"

#ifdef HTML_PROGMEM
const char settings_html[] PROGMEM = R"(<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><meta name="theme-color" content="#abc0df" /><meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1.0, user-scalable=no' /><meta http-equiv="Cache-Control" content="no-cache, no-store, must-revalidate" /><meta http-equiv="Pragma" content="no-cache" /><title>Настройки</title><link rel="stylesheet" type="text/css" href="global.css"><style>input:focus{background:#FA6;outline:none}table{width:100%}td{width:50%}input,select{width:100%;text-align:right}input[type=submit]{width:auto;box-shadow:1px 4px 8px 1px #585858}input[type=checkbox]{width:auto}select{width:100%;text-align-last:right;height:auto;border:1px solid #ccc}.btxt{text-decoration:none;box-shadow:1px 4px 8px 1px #585858;padding:3px}</style> <script>var d=document,w=null;function _(e){return d.getElementById(e);} function _gs(){var r=new XMLHttpRequest();r.overrideMimeType('application/json');r.onreadystatechange=function(){if(r.readyState===4){if(r.status===200){try{var j=JSON.parse(r.responseText);var s=j.scale;for(e in s){try{_(e).value=s[e];}catch(e){}} var i=j.info;for(e in i){try{_(e).innerHTML=i[e];}catch(e){}}}catch(e){alert("ОШИБКА "+e.toString());}}else{alert("ДАННЫЕ НАСТРОЕК НЕ НАЙДЕНЫ!!!");}}};r.open('GET','/settings.json',true);r.send(null);} function formNet(i){var f=new FormData(_(i));var r=new XMLHttpRequest();r.onreadystatechange=function(){if(r.readyState===4){if(r.status===200){var rec=confirm('Перегрузить');if(rec){r.onreadystatechange=null;r.open('GET','/rc',true);r.send(null);}}else if(r.status===400){alert('Настройки не изменились');}}};r.open('POST','/settings.html',true);r.send(f);} function SS(h,p,fm,fe){let tw,ws;this.snd=function(c){ws.send(c);};this.create=function(){this.oS();};this.close=function(){ws.close();};this.wT=function(){clearTimeout(tw);tw=setTimeout(function(){w.close();w.create();},7000);};this.oS=function(){ws=new WebSocket(h,p);ws.onopen=function(){d.body.style.visibility='visible';};ws.onclose=function(){fe();};ws.onerror=function(){fe();};ws.onmessage=function(e){fm(JSON.parse(e.data));}};} function parseCommand(e){if(e.hasOwnProperty('cmd')){switch(e.cmd){case'ssl':listSSID(e);break;case'cls':if(e.code===1111){alert("Лимит клиентов",function(){_('id_alr').style.display='none';},5000);d.body.style.visibility='hidden';} w.close();break;}}} window.onload=function(){w=new SS('ws://'+d.location.host+'/ws',['scales'],parseCommand,function(){w.wT();});w.oS();_gs();};</script> </head><body style='visibility: hidden'> <a href='/'><img src='und.png' alt="&lt;" class='btn btn--s btn--blue'></a>&nbsp;&nbsp;<strong>НАСТРОЙКИ</strong><hr><fieldset style='width: auto'> <details> <summary>ОБЩИИ НАСТРОЙКИ</summary><h3><form id='form_assid_id' action='javascript:formNet("form_assid_id")'><h5 align='left'><b>Имя собственное</b></h5><table><tr><td>ИМЯ:</td><td> <input id='id_assid' name='assid'></td></tr></table> <br><br><h5>Доступ к настройкам</h5><table><tr><td>ИМЯ:</td><td> <input id='id_nadmin' name='nadmin' placeholder='имя админ'></td></tr><tr><td>ПАРОЛЬ:</td><td> <input type='password' id='id_padmin' name='padmin' placeholder='пароль админ'></td></tr></table> <input type='submit' value='СОХРАНИТЬ'/></form></h3><hr> </details></fieldset> <br/><fieldset> <details> <summary>ИНФОРМАЦИЯ</summary> <br><span style='font-size: small; font-weight: bold; color: #626262'><h3><table><tr><td>host:</td><td align='right' id='id_local_host'/></tr><tr><td>динамический iP:</td><td align='right' id='id_sta_ip'/></tr><tr><td>точка:</td><td align='right' id='id_net'/></tr><tr><td>iP точки:</td><td align='right' id='id_ap_ip'/></tr><tr><td>mac:</td><td align='right' id='id_mac'/></tr><tr><td>версия:</td><td align='right'><div id='id_vr'></div></td></tr></table></h3><hr> </span><table style="width: auto"><tr><td> <a href='/setport.html'><img src='port.png' alt="калибровка" class='btn btn--s btn--blue'></a></td><td> <a href='/points.html'><img src='wifi.png' alt="софт" class='btn btn--s btn--blue'></a></td><td> <a href='/binfo.html'><img src='btr.png' alt="бат" class='btn btn--s btn--blue'></a></td></tr></table> </details></fieldset><hr> <footer>2018 © Powered by www.scale.in.ua</footer></body></html>)";
//const char settings_html[] PROGMEM = R"(<!DOCTYPE html><html lang='en'><head> <meta charset='UTF-8'> <meta name="theme-color" content="#abc0df"/> <meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1.0, user-scalable=no'/> <meta http-equiv="Cache-Control" content="no-cache, no-store, must-revalidate"/> <meta http-equiv="Pragma" content="no-cache"/> <title>Настройки</title> <link rel="stylesheet" type="text/css" href="global.css"> <style>input:focus{background: #FA6; outline: none;}table{width: 100%;}td{width: 50%;}input, select{width: 100%; text-align: right;}input[type=submit]{width: auto; box-shadow: 1px 4px 8px 1px #585858;}input[type=checkbox]{width: auto;}select{width: 100%; text-align-last: right; height: auto; border: 1px solid #ccc;}.btxt{text-decoration: none; box-shadow: 1px 4px 8px 1px #585858; padding: 3px;}</style> <script>var d=document, w=null; function _(e){return d.getElementById(e);}function formNet(i){var f=new FormData(_(i)); var r=new XMLHttpRequest(); r.onreadystatechange=function(){if (r.readyState===4){if (r.status===200){var rec=confirm('Перегрузить'); if (rec){r.onreadystatechange=null; r.open('GET', '/rc', true); r.send(null);}}else if (r.status===400){alert('Настройки не изменились');}}}; r.open('POST', '/settings.html', true); r.send(f);}function SS(h, p, fm, fe){let tw, ws; this.snd=function(c){ws.send(c);}; this.create=function(){this.oS();}; this.close=function(){ws.close();}; this.wT=function(){clearTimeout(tw); tw=setTimeout(function(){w.close(); w.create();}, 7000);}; this.oS=function(){ws=new WebSocket(h, p); ws.onopen=function(){d.body.style.visibility='visible';}; ws.onclose=function(){fe();}; ws.onerror=function(){fe();}; ws.onmessage=function(e){fm(JSON.parse(e.data));}};}function parseCommand(e){if (e.hasOwnProperty('cmd')){switch (e.cmd){case 'ssl': listSSID(e); break; case 'cls': if (e.code===1111){alert("Лимит клиентов", function(){_('id_alr').style.display='none';}, 5000); d.body.style.visibility='hidden';}w.close(); break;}}}window.onload=function(){w=new SS('ws://' + d.location.host + '/ws', ['scales'], parseCommand, function(){w.wT();}); w.oS();}; </script></head><body style='visibility: hidden'> <a href='/'><img src='und.png' alt="&lt;" class='btn btn--s btn--blue'></a>&nbsp;&nbsp;<strong>НАСТРОЙКИ</strong> <hr> <fieldset style='width: auto'> <details> <summary>ОБЩИИ НАСТРОЙКИ</summary> <h3> <form id='form_assid_id' action='javascript:formNet("form_assid_id")'> <h5 align='left'><b>Имя собственное</b></h5> <table> <tr> <td>ИМЯ:</td><td> <input id='id_assid' name='assid' value='%assid%'> </td></tr></table> <br><br><h5>Доступ к настройкам</h5> <table> <tr> <td>ИМЯ:</td><td> <input id='id_nadmin' name='nadmin' placeholder='имя админ' value='%nadmin%'> </td></tr><tr> <td>ПАРОЛЬ:</td><td> <input type='password' id='id_padmin' name='padmin' placeholder='пароль админ' value='%padmin%'> </td></tr></table> <input type='submit' value='СОХРАНИТЬ'/></form> </h3> <hr> </details> </fieldset> <br/> <fieldset> <details> <summary>ИНФОРМАЦИЯ</summary> <br><span style='font-size: small; font-weight: bold; color: #626262'> <h3> <table> <tr> <td>host:</td><td align='right' id='id_local_host'/>$local_host$ </tr><tr> <td>динамический iP:</td><td align='right' id='id_sta_ip'/>$sta_ip$ </tr><tr> <td>точка:</td><td align='right' id='id_net'/>$net$ </tr><tr> <td>iP точки:</td><td align='right' id='id_ap_ip'/>$ap_ip$ </tr><tr> <td>mac:</td><td align='right' id='id_mac'/>$mac$ </tr><tr> <td>версия:</td><td align='right'><div id='id_vr'>$vr$</div></td></tr></table> </h3> <hr> </span> <table style="width: auto"> <tr> <td> <a href='/setport.html'><img src='port.png' alt="калибровка" class='btn btn--s btn--blue'></a> </td><td> <a href='/points.html'><img src='wifi.png' alt="софт" class='btn btn--s btn--blue'></a> </td><td> <a href='/binfo.html'><img src='btr.png' alt="бат" class='btn btn--s btn--blue'></a> </td></tr></table> </details> </fieldset> <hr> <footer>2018 © Powered by www.scale.in.ua</footer></body></html>)";
#endif //HTML_PROGMEM

const char netIndex[] PROGMEM = R"(<html lang='en'><meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1'/><body><form method='POST'><input name='ssid'><br/><input type='password' name='key'><br/><input type='submit' value='СОХРАНИТЬ'></form></body></html>)";

class SettingsPageClass : public AsyncWebHandler {
private:
	settings_t * _value;
public:
	SettingsPageClass(settings_t * value)	: _value(value) {};
	virtual bool canHandle(AsyncWebServerRequest *request) override final;
	virtual void handleRequest(AsyncWebServerRequest *request) override final;
	virtual bool isRequestHandlerTrivial() override final {return false;};	
	//String processor(const String& var);
	void handleValue(AsyncWebServerRequest * request);
	size_t doSettings(JsonObject &root);
};

extern SettingsPageClass * SettingsPage;