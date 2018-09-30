/*
  Basic Ticker usage
  
  Ticker is an object that will call a given function with a certain period.
  Each Ticker calls one function. You can have as many Tickers as you like,
  memory being the only limitation.
  
  A function may be attached to a ticker and detached from the ticker.
  There are two variants of the attach function: attach and attach_ms.
  The first one takes period in seconds, the second one in milliseconds.
  
  An LED connected to GPIO1 will be blinking. Use a built-in LED on ESP-01
  or connect an external one to TXD on other boards.
*/

#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

#define STATUS_LED_PIN 4
#define ADC_LIMIT  550
#define WIFI_RUNNING_TIME (60*2)  
#define LIGHT_STRENGTH_CONTINOUS_TIME_LIMIT (20)


ESP8266WebServer server(80);
Ticker status_led_ticker;
Ticker light_strength_ticker;
Ticker wifi_enable_ticker;
//ESP8266WebServer server(80);
/* Set these to your desired credentials. */
const char *ssid = "YEP_ESP";
const char *password = "yep123";

int light_enough_counter = 0;
int status_led_working = 0;
int isWifiSocketStarted = 0;

void startWifiSocket(){
     Serial.println("startWifiSocket()>> ");
     Serial.print("Configuring access point...");
	/* You can remove the password parameter if you want the AP to be open. */
	WiFi.softAP(ssid, password);

	IPAddress myIP = WiFi.softAPIP(); 
	Serial.print("AP IP address: ");
	Serial.println(myIP);
	server.on("/", handleRoot);
	 server.begin();

     Serial.println("startWifiSocket()<< ");

}

void stopWifiSocket(){

	 server.stop();
	 Serial.println("HTTP server stopped>> Rebooting the system");
	 /* this is to reboot system */
	 ESP.deepSleep(5000000, WAKE_RF_DEFAULT);
}

void startWifiSocketTimer(){
    startWifiSocket();
    wifi_enable_ticker.attach(WIFI_RUNNING_TIME, stopWifiSocket);
}


void handleRoot() {
        int adc_value = analogRead(A0);
	server.send(200, "text/html", "<h1>You are connected to YEP WIFI: with ADC: " + String(adc_value) + "</h1>");
}

void status_led_flip() {
  int state = digitalRead(STATUS_LED_PIN);  // get the current state of GPIO1 pin
  digitalWrite(STATUS_LED_PIN, !state);     // set pin to the opposite state
  status_led_ticker.attach(0.3, status_led_flip);
}

void checking_ADC() {
 int adc_value = analogRead(A0);
 Serial.println("ADC VALUE: "+String(adc_value));

 if (adc_value >= ADC_LIMIT){
     light_enough_counter++;
	 if(light_enough_counter > LIGHT_STRENGTH_CONTINOUS_TIME_LIMIT){

		   if(isWifiSocketStarted ==0 ){
				Serial.println("checking_ADC()>> starting http server...");
			   startWifiSocketTimer();
               isWifiSocketStarted = 1;
		   }
	 }
 }else{
   if(isWifiSocketStarted ==0 ){
	 Serial.println("No enough light, go sleep and reboot");
	 /* this is to reboot system */
	 ESP.deepSleep(5000000, WAKE_RF_DEFAULT);
   }
 }
}

void setup() {
	Serial.begin(115200);
    Serial.println("setup()>> ");
    isWifiSocketStarted = 0;

  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(A0, INPUT);

  digitalWrite(STATUS_LED_PIN, LOW);
  
  // status_led_flip the pin every 0.3s
  status_led_ticker.attach(0.3, status_led_flip);
  light_strength_ticker.attach(0.2, checking_ADC);
 }

void loop() {
	 server.handleClient();
}
