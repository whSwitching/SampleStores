/*
  ESP-01S IO2: GPIO2/UART1_TXD/blueLED
  ESP-01S IO0: GPIO0 下载模式:外部拉低, 运行模式:悬空或者外部拉高
  ESP-01S RXD: UART0_RXD/GPIO3 Serial.begin
  ESP-01S TXD: UART0_TXD/GPIO1
  ESP-01S-Relay: 连接GPIO0, 低电平触发
*/
#define BLINKER_WIFI
#define BLINKER_WITHOUT_SSL
#define BLINKER_ALIGENIE_LIGHT
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>  // https://github.com/esp8266/Arduino
#include <ESP8266WebServer.h>
#include <Blinker.h>  // https://github.com/blinker-iot/blinker-library

#define WIFI_SSID "<your wifi ssid>"
#define WIFI_PASSWD "<your wifi password>"
#define Blinker_DEVICE_SECRET "<blinker device secret>"  // from app https://github.com/blinker-iot/app-release
IPAddress WIFI_IP(192, 168, 2, 66);     // use static ip so we can find it easily
IPAddress WIFI_MASK(255, 255, 255, 0);  // subnet mask
IPAddress WIFI_GATE(192, 168, 2, 1);    // gateway
IPAddress WIFI_DNS1(180, 76, 76, 76);   // dns 1
IPAddress WIFI_DNS2(8, 8, 8, 8);        // dns 2

const int RELAY_PIN = 0; //GPIO0
const int LED_PIN = 2; //GPIO2
ESP8266WebServer webserver(80);
char Blinker_auth[] = Blinker_DEVICE_SECRET;
char Blinker_ssid[] = WIFI_SSID;
char Blinker_pwd[] = WIFI_PASSWD;
BlinkerButton btnGpio0("btn-gpio0");    // a switch button component name, from app https://github.com/blinker-iot/app-release

void wifiInit()
{
  Serial.println();
  Serial.println("WIFI Init...");
  Serial.print("  local ip = ");
  Serial.println(WIFI_IP);
  Serial.print("  subnet mask = ");
  Serial.println(WIFI_MASK);
  Serial.print("  gateway = ");
  Serial.println(WIFI_GATE);
  Serial.print("  dns1 = ");
  Serial.println(WIFI_DNS1);
  Serial.print("  dns2 = ");
  Serial.println(WIFI_DNS2);
  WiFi.config(WIFI_IP, WIFI_GATE, WIFI_MASK, WIFI_DNS1, WIFI_DNS2);
  Serial.println("  mode = WIFI_STA");
  WiFi.mode(WIFI_STA);
  Serial.println("  Connecting SSID " + String(WIFI_SSID));
  WiFi.begin(WIFI_SSID, WIFI_PASSWD);
  WiFi.setAutoConnect (true);
  WiFi.setAutoReconnect (true);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println(".");
  }
  Serial.println("WIFI Connected");
  Serial.println(WiFi.localIP());
}

void webserverInit()
{
  Serial.println();
  Serial.println("WebServer Init...");
  webserver.begin();
  webserver.on("/", webserverOnRoot);
  webserver.on("/gpio", webserverOnGpio);
  webserver.onNotFound(webserverOnNotFound);
  Serial.println("WebServer Setup.");
}

void webserverSerialOut(String resp)
{
  Serial.println();
  Serial.print("WebServer Request: ");
  Serial.print((webserver.method() == HTTP_POST) ? "POST " : "GET ");
  Serial.println(webserver.uri());
  int c = webserver.args();
  if (c > 0)
  {
    Serial.print("Query: ");
    for (int i = 0; i < c; i++)
    {
      Serial.print(webserver.argName(i) + "=" + webserver.arg(i));
      if (i < c - 1)
        Serial.print(" & ");
      else
        Serial.println();
    }
  }
  Serial.println("Response: " + resp);
}

void webserverOnNotFound()
{
  StaticJsonDocument<512> doc;
  JsonObject obj = doc.to<JsonObject>();
  obj["StatusCode"] = 404;
  obj["URL"] = webserver.uri();
  obj["Method"] = (webserver.method() == HTTP_POST) ? "POST" : "GET";
  String json;
  serializeJson(doc, json);
  webserver.send(404, "application/json", json);
  webserverSerialOut(json);
}

void webserverOnRoot()
{
  StaticJsonDocument<512> doc;
  JsonObject obj = doc.to<JsonObject>();
  obj["StatusCode"] = 200;
  obj["WIFI_SSID"] = String(WIFI_SSID);
  obj["swi"] = getRelayStateString();
  String json;
  serializeJson(doc, json);
  webserver.send(200, "application/json", json);
  webserverSerialOut(json);
}

void webserverOnGpio()
{
  setRelayState(webserver.arg("swi"));
  String json = getRelayStateJson();
  webserver.send(200, "application/json", json);
  webserverSerialOut(json);
}

void blinkerInit()
{
  Serial.println();
  Serial.println("Blinker Init...");
  BLINKER_DEBUG.stream(Serial);
  BLINKER_DEBUG.debugAll();
  Blinker.begin(Blinker_auth, Blinker_ssid, Blinker_pwd);
  Blinker.attachHeartbeat(blinker_heartbeat);
  btnGpio0.attach(blinker_btnGpio0_callback);
  BlinkerAliGenie.attachPowerState(blinker_genie_callback);
  BlinkerAliGenie.attachQuery(blinker_genie_query);
  Serial.println("Blinker Setup.");
  Serial.println(WiFi.localIP());
}

void blinker_heartbeat()
{
  btnGpio0.print(getRelayStateString());
  Serial.println();
  Serial.println("Blinker heartbeat");
}

void blinker_btnGpio0_callback(const String & btnCmd)
{
  setRelayState(btnCmd);
  Serial.println();
  Serial.println("Blinker AppGui Request: btn_gpio0 " + btnCmd);
  String swiState = getRelayStateString();
  btnGpio0.print(swiState);
  Serial.println("  Response: " + swiState);
}

void blinker_genie_callback(const String & genieCmd)
{
  setRelayState(genieCmd);
  Serial.println();
  Serial.println("Blinker AliGenie Request: PowerState " + genieCmd);
  String swiState = getRelayStateString();
  BlinkerAliGenie.powerState(swiState);
  BlinkerAliGenie.print();
  Serial.println("  Response: " + swiState);
}

void blinker_genie_query(int32_t queryCode)
{
  String swiState = getRelayStateString();
  switch (queryCode)
  {
    case BLINKER_CMD_QUERY_ALL_NUMBER:
    case BLINKER_CMD_QUERY_POWERSTATE_NUMBER :
      BlinkerAliGenie.powerState(swiState);
      BlinkerAliGenie.print();
      break;
    default :
      break;
  }
  Serial.println();
  Serial.println("Blinker AliGenie query: Code=" + String(queryCode));
  Serial.println("  Response: " + swiState);
}

String getRelayStateString()
{
  return (digitalRead(RELAY_PIN) == 0) ? "on" : "off";
}
String getRelayStateJson()
{
  return (digitalRead(RELAY_PIN) == 0) ? "{\"swi\":\"on\"}" : "{\"swi\":\"off\"}";
}
void setRelayState(const String & stat)
{
  if (stat == BLINKER_CMD_ON || stat == "1")
  {
    digitalWrite(RELAY_PIN, LOW);
  }
  else if (stat == BLINKER_CMD_OFF || stat == "0")
  {
    digitalWrite(RELAY_PIN, HIGH);
  }
}

void setup()
{
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  Serial.begin(115200);
  wifiInit();
  webserverInit();
  blinkerInit();
}

void loop()
{
  webserver.handleClient();
  Blinker.run();
}
