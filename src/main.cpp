// ESP32 Webserver, AP mode, STA mode, FRNG, TRNG, 2D Ploter, 3D Cosmos, Chess
// IDE PlatformIO (13.04.2024)

/* Build
RAM:   [=         ]  13.8% (used 45244 bytes from 327680 bytes)
Flash: [======    ]  61.6% (used 807041 bytes from 1310720 bytes)
*/

#include <WiFi.h>
#include <esp_wifi.h>           // FOR esp_wifi_set_mac
#include <esp_timer.h>
#include <ESPAsyncWebServer.h>
#include <webPage.h>

#define GP04    4               // User Led (Blue)
#define GP16    16              // LOW - Out TX Enable/Disable
#define GP17    17              // Button (Set AP Mode)
#define GP34    34              // ADC (add to pin wire 0.1 m)

typedef unsigned char   u8;
typedef signed char     i8;
typedef unsigned short  u16;
typedef signed short    i16;
typedef unsigned long   u32;
typedef signed long     i32;

void webserver_Init_STA_Mode();
void webserver_Init_AP_Mode();

AsyncWebServer server(80);      // Set AsyncWebServer Port 80

IPAddress   sta_IP(192, 168, 0, 220);
IPAddress   sta_GW(192, 168, 0, 1);
IPAddress   sta_Mask(255, 255, 255, 0);
IPAddress   ap_IP(192, 168, 0, 220);
IPAddress   ap_Mask(255, 255, 255, 0);
String      ssid_sta      = "ssid";
String      pass_sta      = "pass";
const char* ssid_ap       = "ESP32";
const char* pass_ap       = "12345678";
boolean     ap_Mode       = false;
boolean     sta_Connect   = false;
boolean     webserver_On  = false;
hw_timer_t  *timer        = NULL;
u32         rnd_Micros    = 0;
u16         i             = 0;
u8          arr_x[1000];
u8          arr_y[1000];
String      s_arr_x;
String      s_arr_y;

// FRNG - Fast Random Numbers Generanor - https://youtu.be/G6MROvlLeKE?t=456
static u16 x=12345, y=6789, z=42, w=1729;
u16 rng1=0, rng2=0, rng3=0;
u16 xorshift16() {u16 t=x^(x<<5); x=y; y=z; z=w; w=w^(w>>1)^t^(t>>3); return w&0xFFFF;}

// TRNG - True Random Numbers Generanor
u16 port, buf=0, adc=0;
u8 TRNG_Bit(void);
void TRNG_Init(u16 p) {port=p; for (u8 i=0; i<12; i++) {buf=buf<<1|TRNG_Bit();}}
u8 TRNG_Bit(void) {delay(1); adc=analogRead(port)&1; buf=buf<<1|adc; return (buf&1)^(buf>>1&1)^(buf>>2&1)^(buf>>10&1);}
u8 TRNG_Byte(void) {u8 r=0; for (u8 i=0; i<8; i++) {r=r<<1|TRNG_Bit();} return r;}
u8 TRNG_Byte(u8 max) {return TRNG_Byte()*max/0xff;}
u16 TRNG_Word(void) {u16 r=0; for (u8 i=0; i<2; i++) {r=r<<8|TRNG_Byte();} return r;}
u16 TRNG_Word(u16 max) {return TRNG_Word()*max/0xffff;}
u32 TRNG_Dword(void) {u32 r=0; for (u8 i=0; i<2; i++) {r=r<<16|TRNG_Word();} return r;}
u32 TRNG_Dword(u32 max) {return TRNG_Dword()*max/0xffffffff;}

void ledOn()  {digitalWrite(GP04, HIGH);} // User Led (Blue)
void ledOff() {digitalWrite(GP04, LOW);}  // User Led (Blue)

void IRAM_ATTR restart()  {
  Serial.println("RESTART");
  delay(100); digitalWrite(GP16, LOW);    // HIGH - Out TX Disable
  delay(100); esp_restart();
}

void timer_1s() {
	static unsigned long timerTik = 0;      // Reset Timer (Feed Watchdog < 3s)
	if ((millis()-timerTik) > 1000) {timerTik = millis(); timerWrite(timer, 0);}
}

void flashLed(uint8_t n) {
  while (n--) {
    ledOn();  delay(10);
    ledOff(); delay(190);
  }
}

void rnd() {
  //int rnd = xorshift16() >> 8;
  //u16 rnd = xorshift16();
  //Serial.println(rnd, DEC);
  //if (rnd == 0) {Serial.println("Stop 0"); delay(10000);}       // Yes
  //if (rnd == 255) {Serial.println("Stop 255"); delay(10000);}   // Yes
  
  //adc = analogRead(GP34);
  
  //u8 rng1 = TRNG_Byte();
  //u8 rng2 = TRNG_Byte();
  //u8 rng3 = TRNG_Byte();

  rng1 = xorshift16();
  rng2 = xorshift16();
  rng3 = xorshift16();
  // Format: SPP - Serial Port Plotter
  Serial.println("$"+String(rng1)+" "+String(70000+rng2)+" "+String(140000+rng3)+";");
  arr_x[i] = rng1;
  arr_y[i] = rng2;
  if (i < 1000) {i++;} else {
    i = 0;
    s_arr_x = "";
    s_arr_y = "";
    for (uint16_t j=0; j<1000; j++) {
      s_arr_x += String(arr_x[j])+",";
      s_arr_y += String(arr_y[j])+",";
    }
  }
}

String processor_index(const String& var) {
  if (var == "rng1") {return String(rng1);}
  if (var == "rng2") {return String(rng2);}
  if (var == "rng3") {return String(rng3);}
  if (var == "connect") {
    if (ap_Mode) {return String("<a href='sta'>Connect to STA mode</a>");}
    else {return String("<a href='ap'>Connect to AP mode</a>");}
  }
  return String("");
}

String processor_plot(const String& var) {
  if (var == "arr_x") {return String(s_arr_x);}
  if (var == "arr_y") {return String(s_arr_y);}
  return String("");
}

void page_Index(AsyncWebServerRequest *request) {
  ledOn();
  Serial.println("Index Page");
  request->send_P(200, "text/html", index_html, processor_index);
  ledOff();
}

void page_Plot(AsyncWebServerRequest *request) {
  ledOn();
  Serial.println("Plot Page");
  request->send_P(200, "text/html", plot_html, processor_plot);
  ledOff();
}

void page_Cosmos(AsyncWebServerRequest *request) {
  ledOn();
  Serial.println("Cosmos Page");
  request->send_P(200, "text/html", cosmos_html);
  ledOff();
}

void page_Chess(AsyncWebServerRequest *request) {
  ledOn();
  Serial.println("Chess Page");
  request->send_P(200, "text/html", chess_html);
  ledOff();
}

void page_STA(AsyncWebServerRequest *request) {
  ledOn();
  Serial.println("STA Page");
  ap_Mode = false;
  request->send_P(200, "text/html", index_html, processor_index);
  ledOff();
  flashLed(2);
  restart();
}

void page_AP(AsyncWebServerRequest *request) {
  ledOn();
  Serial.println("AP Page");
  ap_Mode = true;
  request->send_P(200, "text/html", index_html, processor_index);
  ledOff();
  flashLed(2);
  webserver_On = false;
  timerWrite(timer, 0);
  webserver_Init_AP_Mode();
}

void page_Rng(AsyncWebServerRequest *request) {
  ledOn();
  String arr = String(rng1)+"/"+String(rng2)+"/"+String(rng3);   // rng1/rng2/rng3
  request->send_P(200, "text/plain", String(arr).c_str());
  ledOff();
}

void page_NotFound(AsyncWebServerRequest *request) {
  ledOn();
  Serial.println("NotFound Page");
  request->send_P(200, "text/html", p404_html);
  ledOff();
}

void start_Webserver() {
  server.on("/",       HTTP_GET, page_Index);
  server.on("/plot",   HTTP_GET, page_Plot);
  server.on("/cosmos", HTTP_GET, page_Cosmos);
  server.on("/chess",  HTTP_GET, page_Chess);
  server.on("/sta",    HTTP_GET, page_STA);
  server.on("/ap",     HTTP_GET, page_AP);
  server.on("/rng",    HTTP_GET, page_Rng);
  server.onNotFound(page_NotFound);
  server.begin();
  webserver_On = true;
  Serial.println("Webserver On");
  flashLed(2);
}

void webserver_Init_STA_Mode() {
  ap_Mode = false;
  WiFi.disconnect();
  delay(100);
  WiFi.mode(WIFI_STA);
  WiFi.begin((String)ssid_sta, (String)pass_sta);
  WiFi.config(sta_IP, sta_GW, sta_Mask);
  if (!webserver_On) {start_Webserver();}
  Serial.println("\nWiFi STA Mode");
  Serial.print("SSID: ");       Serial.println(ssid_sta);
  Serial.print("Password: ");   Serial.println(pass_sta);
  Serial.print("MAC: ");        Serial.println(WiFi.macAddress());
}

void webserver_Init_AP_Mode() {
  ap_Mode = true;
  WiFi.disconnect();
  delay(100);
  WiFi.mode(WIFI_AP);
  WiFi.softAP((char*)ssid_ap, (char*)pass_ap);
  WiFi.softAPConfig(ap_IP, ap_IP, ap_Mask);
  if (!webserver_On) {start_Webserver();}
  ledOn();
  Serial.println("\nWiFi AP Mode");
  Serial.print("SSID: ");       Serial.println((String)ssid_ap);
  Serial.print("Password: ");   Serial.println((String)pass_ap);
  Serial.print("MAC: ");        Serial.println(WiFi.macAddress());
  Serial.print("WEB Server: "); Serial.println(WiFi.softAPIP());
}

void ap_Test_Buton() {
  if (digitalRead(GP17)) {return;}
  delay(50);
  if (digitalRead(GP17)) {return;}
  delay(50);
  if (digitalRead(GP17)) {return;}
  ledOn();
  Serial.println("AP Mode");
  while (!digitalRead(GP17)) {delay(50);}
  webserver_Init_AP_Mode();
}

void sta_Test_Connect() {
  if (WiFi.status() != WL_CONNECTED) {sta_Connect = false; return;}
  rnd_Micros = micros();    // 807871, 806877, 806874, 805898, 805891
  if (!webserver_On) {start_Webserver();}
  if (!sta_Connect) {
    Serial.print("\nWiFi Connected! Signal RSSI: "); Serial.println(WiFi.RSSI());
    Serial.print("WEB Server: "); Serial.println(WiFi.localIP());
    Serial.print("RND Micros: "); Serial.println(rnd_Micros);
    sta_Connect = true;
  }
}

void setup() {
  esp_deep_sleep_disable_rom_logging();   // Disable Bootloader logging
  //setCpuFrequencyMhz(80);               // {240, 160, 80}

  pinMode(GP34, ANALOG);                  // ADC
  pinMode(GP17, INPUT_PULLUP);            // Button (Set AP Mode)
  pinMode(GP04, OUTPUT);                  // User Led (Blue)
  digitalWrite(GP04, LOW);                // User Led (Blue) Off
  pinMode(GP16, OUTPUT);
  digitalWrite(GP16, LOW);                // HIGH - Out TX Disable
  delay(100);

  Serial.setDebugOutput(false);
  Serial.begin(115200);
  while (!Serial);
  digitalWrite(GP16, HIGH);               // HIGH - Out TX Enable
  delay(10);
  if (!digitalRead(GP17)) {webserver_Init_AP_Mode();}
  else                    {webserver_Init_STA_Mode();}

  timer = timerBegin(0, 80, true);              // Timer 0, div 80
  timerAttachInterrupt(timer, &restart, true);  // Attach Callback
  timerAlarmWrite(timer, 3000000, false);       // Set Watchdog Timeout 3s (3000000us)
  timerAlarmEnable(timer);                      // Enable Interrupt

  TRNG_Init(GP34);
}

void loop() {
  rnd();
  timer_1s();
  if (!ap_Mode) {ap_Test_Buton();}        // Перевірка переходу в AP режим
  if (!ap_Mode) {sta_Test_Connect();}     // Перевірка WiFi коннекту в режимі STA
}
