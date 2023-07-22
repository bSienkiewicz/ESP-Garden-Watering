#ifndef INDEX_H
#define INDEX_H

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang=en>
<head>
<title>Plantwise</title>
<meta name=viewport content='width=device-width, initial-scale=1.0'>
<style>*{margin:0}body{font-family:sans-serif;display:flex;flex-direction:column;align-items:center;text-align:center}small{color:gray}label,h3{margin-top:20px}.sh{width:30px}</style>
</head>
<body>
<svg xmlns=http://www.w3.org/2000/svg width=80 height=80 fill=none viewBox='0 0 292 265'><g clip-path=url(#a)><path fill=#38B8B1 d='M181 199c-8 1-13-1-17-8-4-6-3-12 0-18l1-2v-3h-3l-2 2c-5 8-5 16-1 24v6c-9 11-23 11-31 2l-2-1c-2-2-3-2-5 0l-5 6c-6 4-16 4-21-2-2-2-2-3 0-5l9-11 5-1c7 1 13-1 18-6l3-3v-3h-3l-2 2c-4 4-8 5-13 5-3 0-4-1-4-4l1-4-1-2c-1-1-2 0-3 1l-1 3c-3 11-11 20-22 25-11 6-23 5-34 0-3-1-4-3-4-6-1-12 3-22 12-31 3-3 4-3 8-1l8 4c3 1 4 1 4 5 1 5-1 10-5 14l-1 1v3h3c5-3 7-7 8-12v-7c0-4 0-4 3-6 4-1 7-3 9-6l1-3v-2h-3l-2 2c-7 8-17 9-24 2-6-4-8-10-6-17 1-6 5-10 11-13 2 0 3 0 4 2l6 4 2 2 3-1c1-1 0-2-1-3l-1-1c-12-9-12-27 0-34l7-4c2 0 4 0 5 2 6 7 13 10 22 9l3-1 1-3-2-1h-3c-9 1-16-3-19-10s0-16 7-21c5-4 12-4 18-2l2 1c2 1 4 0 4-3 2-6 5-10 11-13s13-4 20 0c4 2 4 3 3 7-1 6 0 12 3 16s3 5 1 9l-8 7-4 3v3l2 1 4-2c4-3 8-6 11-11 2-3 2-3 6-3 3 1 7 1 11-1l2-2 1-2-2-1-3 1c-7 3-15 0-18-6-3-7-1-15 5-19 10-7 23-3 29 4 8 9 8 21 2 29-1 2-3 4-6 5l-35 22-22 15-26 20-1 1c-1 1-2 3 0 4h4l5-5c5-4 10-9 16-12l46-31a130 130 0 0 1 69-26c4-1 6 3 4 6l-3 2c-10 0-20 2-29 5-4 2-8 3-11 6l2 1h5c11 2 18 12 19 21 1 10-4 19-14 23-6 2-12 0-16-6-3-5-3-12 1-16l3-3v-3l-3-1-2 2c-3 3-5 6-6 11 0 3-1 3-4 3l-14 5-3 3-1 3h4l6-3 9-3c2 0 4 1 5 3 3 6 8 10 15 11 2 0 3 1 4 4 2 7-2 16-9 20-4 3-9 5-15 4-3 0-5 1-4 4 0 9-6 18-16 19Zm-36-38 2-2c9-10 26-10 35 0l2 6-3 10c0 1-1 3 1 4 2 0 3-1 4-3l1-2 1-7c1-2 2-3 4-3a56 56 0 0 0 11-2c1 0 2-1 2-3l-3-1h-4c-6 1-11 1-14-4h-1c-9-8-19-10-29-6s-16 12-18 22c-1 3 0 4 2 5 2 0 2-2 3-4l4-10Zm-35-33c22 3 39-18 31-39-1-4-1-8 1-12l3-4 1-2-1-2h-3l-2 2-4 8c-1 2-2 2-4 2l-9-2c-3 0-4 1-4 2 0 3 2 3 3 3l10 2c2 0 4 1 4 4l2 5c3 17-11 30-28 28l-3-1-2 2c-1 1 0 2 1 3l4 1Z'/></g><defs><clipPath id=a><path fill=#fff d='M1 135 206 0l85 129L85 264z'/></clipPath></defs></svg>
<h1 style=margin-top:40px>Welcome to Plantwise</h1>
<p>The next step is to configure this device.</p>
<form action=/config method=post style=display:flex;flex-direction:column;align-items:center;text-align:center;margin-top:40px>
<h3>WiFi Setup</h3>
<small>Connection to your WiFi network</small>
<label for=ssid>Network name (SSID)</label>
<input type=text name=ssid id=ssid placeholder=SSID required>
<label for=password>Password</label>
<input type=password name=password id=password placeholder=Password>
<label for=sip>Static IP</label>
<div style=display:flex>
<input type=tel class=sh name=sip_1 placeholder=0 value=0 pattern=^(?:\d{1,2}|1\d{2}|200)$>
<p>.</p>
<input type=tel class=sh name=sip_2 placeholder=0 value=0 pattern=^(?:\d{1,2}|1\d{2}|200)$>
<p>.</p>
<input type=tel class=sh name=sip_3 placeholder=0 value=0 pattern=^(?:\d{1,2}|1\d{2}|200)$>
<p>.</p>
<input type=tel class=sh name=sip_4 placeholder=0 value=0 pattern=^(?:\d{1,2}|1\d{2}|200)$>
</div>
<small>Leave 0.0.0.0 for DHCP</small>
<label for=sip>Gateway</label>
<div style=display:flex>
<input type=tel class=sh name=gateway_1 placeholder=0 value=0 pattern=^(?:\d{1,2}|1\d{2}|200)$>
<p>.</p>
<input type=tel class=sh name=gateway_2 placeholder=0 value=0 pattern=^(?:\d{1,2}|1\d{2}|200)$>
<p>.</p>
<input type=tel class=sh name=gateway_3 placeholder=0 value=0 pattern=^(?:\d{1,2}|1\d{2}|200)$>
<p>.</p>
<input type=tel class=sh name=gateway_4 placeholder=0 value=0 pattern=^(?:\d{1,2}|1\d{2}|200)$>
</div>
<small>Router address</small>
<small>Leave 0.0.0.0 for DHCP</small>
<label for=sip>Channel (1-13)</label>
<input type=tel name=channel placeholder=1 value=1 pattern=^(1[0-3]|[1-9])$ required>
<label for=gateway>mDNS</label>
<div style=display:flex>
<p>http://</p>
<input type=text name=mdns placeholder=plantwise>
<p>.local</p>
</div>
<small>Leave empty for no mDNS</small>
<h3>MQTT Setup</h3>
<label for=mqtt_server>MQTT Server</label>
<div style=display:flex>
<p>mqtt://</p>
<input type=text name=mqtt_server id=mqtt_server placeholder='MQTT Server' required>
<p>&nbsp;:&nbsp;</p>
<select name=mqtt_port id=mqtt_port required>
<option selected value=1883>1883</option>
<option value=8883>8883</option>
</select>
</div>
<label for=mqtt_user>MQTT User</label>
<input type=text name=mqtt_user id=mqtt_user placeholder='MQTT User' required>
<label for=mqtt_password>MQTT Password</label>
<input type=password name=mqtt_password id=mqtt_password placeholder='MQTT Password' required>
<h3>Access Point Setup</h3>
<label for=ap_name>AP Name</label>
<input type=text name=ap_name id=ap_name placeholder='AP Name' required>
<label for=ap_password>AP Password</label>
<input type=password name=ap_password id=ap_password placeholder='AP Password'>
<small>Leave empty for open</small>
<div style=margin-top:20px>
<label for=hide_ap>Hide Access Point</label>
<input type=checkbox name=hide_ap id=hide_ap value=1>
</div>
<small style=color:red>If you are going to hide this device, you need to remember its SSID or IP address to reconnect to it.</small>
<input type=submit value='Finish the configuration' style='padding:8px 8px;background:#38b8b1;border:0;font-weight:bold;margin:20px 0 20px'>
</form>
</body>
</html>
)=====";

#endif