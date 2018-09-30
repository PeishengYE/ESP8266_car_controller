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
#define STATUS_LED_PIN 4
#define ADC_LIMIT  550

Ticker status_led_ticker;
Ticker light_strength_ticker;
//ESP8266WebServer server(80);


int status_led_working = 0;

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

 }else{

     status_led_working = 1;
 }
}

void setup() {
	Serial.begin(115200);
    Serial.println("");

  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(A0, INPUT);

  digitalWrite(STATUS_LED_PIN, LOW);
  
  // flip the pin every 0.3s
  status_led_ticker.attach(0.3, flip);
  light_strength_ticker.attach(0.4, checking_ADC);
}

void loop() {
}
