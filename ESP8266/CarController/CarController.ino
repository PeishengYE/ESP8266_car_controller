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

#define UNLOCK_DOOR_PIN 5
#define STATUS_LED_PIN 4
#define ADC_LIMIT  300
#define WIFI_RUNNING_TIME (60*2)  
#define LIGHT_STRENGTH_CONTINOUS_TIME_LIMIT (20)
#define SYSTEM_DEEP_SLEEPTIME (10000000) /* 10 seconds */
#define ADC_ARRAY_SIZE 30
#define CHANG_MINIMUM 50

ESP8266WebServer server(80);
Ticker status_led_ticker;
Ticker unlock_door_ticker;
Ticker ADC_reader_timer;
Ticker wifi_enable_ticker;
Ticker stop_timer;

//ESP8266WebServer server(80);
/* Set these to your desired credentials. */
const char *ssid = "YEP_ESP";
const char *password = "yep123";
int lights_strengths[ADC_ARRAY_SIZE];

int light_enough_counter = 0;
int status_led_working = 0;
int adc_array_cursor = 0;
int status_light_interval = 500;

void startWifiSocket(){
     Serial.println("startWifiSocket()>> ");
     Serial.print("Configuring access point...");
	/* You can remove the password parameter if you want the AP to be open. */
	WiFi.softAP(ssid, password);

	IPAddress myIP = WiFi.softAPIP(); 
	Serial.print("AP IP address: ");
	Serial.println(myIP);
	server.on("/345678", handleRoot);
	 server.begin();

     Serial.println("startWifiSocket()<< ");

}

void stopWifiSocket(){

	 server.stop();
	 Serial.println("Timeout, HTTP server stopped>> Rebooting the system");
	 /* this is to reboot system */
	 //ESP.deepSleep(5000000, WAKE_RF_DEFAULT);
	 ESP.deepSleep(SYSTEM_DEEP_SLEEPTIME, WAKE_RF_DEFAULT);
}

void startWifiSocketTimer(){
    startWifiSocket();
    wifi_enable_ticker.attach(WIFI_RUNNING_TIME, stopWifiSocket);
}

void unlock_door_pin_to_zero() {
     digitalWrite(UNLOCK_DOOR_PIN, 0);     // set pin to OFF
}
void unlock_door() {
     digitalWrite(UNLOCK_DOOR_PIN, 1);     // set pin to ON
     unlock_door_ticker.once(1, unlock_door_pin_to_zero);
}

void handleRoot() {
        int adc_value = analogRead(A0);
	server.send(200, "text/html", "<h1>You are connected to YEP WIFI: with ADC: " + String(adc_value) + "</h1>");
        Serial.println("handleRoot()<<  Done for Http response ! ");   
        unlock_door();
}


void status_led_flip() {
  int state = digitalRead(STATUS_LED_PIN);  // get the current state of GPIO1 pin
  digitalWrite(STATUS_LED_PIN, !state);     // set pin to the opposite state
  status_led_ticker.attach_ms(status_light_interval, status_led_flip);
}

void check_light_strengths(){
	int diff_prev = 0;
	int diff_next = 0;
	int valley_count = 0;

  ADC_reader_timer.detach();
  
  

  for (int i=1; i< ADC_ARRAY_SIZE -1; i++){
	  diff_prev = lights_strengths[i-1] - lights_strengths[i];
	  diff_next = lights_strengths[i+1] - lights_strengths[i];
	  if((diff_prev > CHANG_MINIMUM) &&(diff_next > CHANG_MINIMUM )){
		  valley_count ++;
	  }
  }
   
   if(valley_count >= 1){
	   /* speed up the lights */
               status_light_interval = 200;
				Serial.println("check_light_strengths()>> find light change, starting http server...");
			   startWifiSocketTimer();
   }else{
	 Serial.println("No enough light change, go sleep and reboot");
	  //this is to reboot system 
	 //ESP.deepSleep(5000000, WAKE_RF_DEFAULT);
	 ESP.deepSleep(SYSTEM_DEEP_SLEEPTIME, WAKE_RF_DEFAULT);

   }

}

void checking_ADC() {
  if (adc_array_cursor <= ADC_ARRAY_SIZE) {
	 int adc_value = analogRead(A0);
	 Serial.println("ADC VALUE: "+String(adc_value));
	 lights_strengths[adc_array_cursor++] = adc_value;
  }
}

/*
void checking_ADC() {
 int adc_value = analogRead(A0);
 Serial.println("ADC VALUE: "+String(adc_value));

 if (adc_value <= ADC_LIMIT){
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
	  //this is to reboot system 
	 //ESP.deepSleep(5000000, WAKE_RF_DEFAULT);
	 ESP.deepSleep(SYSTEM_DEEP_SLEEPTIME, WAKE_RF_DEFAULT);
   }
 }
}
	 */

void setup() {
	Serial.begin(115200);
    Serial.println("setup()>> ");
    adc_array_cursor = 0;

  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(UNLOCK_DOOR_PIN, OUTPUT);
  pinMode(A0, INPUT);
  memset(lights_strengths, 0, sizeof(lights_strengths));

  digitalWrite(STATUS_LED_PIN, LOW);
  digitalWrite(UNLOCK_DOOR_PIN, LOW);
  
  // status_led_flip the pin every 0.3s
  status_led_ticker.attach_ms(status_light_interval, status_led_flip);
  ADC_reader_timer.attach(0.2, checking_ADC);
  stop_timer.once(5, check_light_strengths);
 }

void loop() {
	 server.handleClient();
}
