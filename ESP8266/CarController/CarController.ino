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
ESP8266WebServer server(80);
Ticker status_led_ticker;
Ticker light_strength_ticker;
//ESP8266WebServer server(80);
/* Set these to your desired credentials. */
const char *ssid = "YEP_ESP";
const char *password = "yep123";


int status_led_working = 0;
void handleRoot() {
        int adc_value = analogRead(A0);
	server.send(200, "text/html", "<h1>You are connected to YEP WIFI: with ADC: " + String(adc_value) + "</h1>");
}

void flip() {
  int state = digitalRead(STATUS_LED_PIN);  // get the current state of GPIO1 pin
  digitalWrite(STATUS_LED_PIN, !state);     // set pin to the opposite state
  
  if (status_led_working == 1){
  // make flash crazy
    status_led_ticker.attach(0.1, flip);


  } else {
  // make flash at normal speed
    status_led_ticker.attach(0.3, flip);

  }

}

void checking_ADC() {
 int adc_value = analogRead(A0);
 Serial.println("ADC VALUE: "+String(adc_value));

 if (adc_value >= ADC_LIMIT){
     status_led_working = 0;
	 server.begin();
	 Serial.println("HTTP server started");


 }else{

     status_led_working = 1;
	 server.stop();
	 Serial.println("HTTP server stopped>> deepsleep for 5s with WAKE_RF_DEFAULT");
	 ESP.deepSleep(5000000, WAKE_RF_DEFAULT);
 }
}

void setup() {
	Serial.begin(115200);
    Serial.println("setup()>> ");

  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(A0, INPUT);

  digitalWrite(STATUS_LED_PIN, LOW);
  
  // flip the pin every 0.3s
  status_led_ticker.attach(0.3, flip);
  light_strength_ticker.attach(0.4, checking_ADC);
      Serial.print("Configuring access point...");
	/* You can remove the password parameter if you want the AP to be open. */
	WiFi.softAP(ssid, password);

	IPAddress myIP = WiFi.softAPIP(); 
	Serial.print("AP IP address: ");
	Serial.println(myIP);
	server.on("/", handleRoot);
      Serial.println("setup()<< ");

}

void loop() {
	 server.handleClient();
}
