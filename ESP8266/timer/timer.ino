

/*
    ESP8266 Timer Example
    Hardware: NodeMCU
    Circuits4you.com
    2018
    LED Blinking using Timer
*/
#include <ESP8266WiFi.h>
#include <Ticker.h>
 #include <WiFiClient.h> 
#include <ESP8266WebServer.h>
/* Set these to your desired credentials. */
const char *ssid = "YEP_ESP";
const char *password = "yep123";

Ticker blinker;
 
#define LED 4  //On board LED
 ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void handleRoot() {
	server.send(200, "text/html", "<h1>You are connected to YEP WIFI</h1>");
}
//=======================================================================
void ICACHE_RAM_ATTR onTimerISR(){
    digitalWrite(LED,!(digitalRead(LED)));  //Toggle LED Pin
    timer1_write(60000000);//12us
     int raw = analogRead(A0);
     Serial.println("ADC VALUE: "+String(raw));
     
}
//=======================================================================
//                               Setup
//=======================================================================
void setup()
{
    Serial.begin(115200);
    Serial.println("");
 
    pinMode(LED,OUTPUT);
 
    pinMode(A0, INPUT);
    int raw = analogRead(A0);
    //Initialize Ticker every 0.5s
    timer1_attachInterrupt(onTimerISR);
    timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
    timer1_write(60000000); //120000 us
    Serial.print("Configuring access point...");
	/* You can remove the password parameter if you want the AP to be open. */
	WiFi.softAP(ssid, password);

	IPAddress myIP = WiFi.softAPIP(); 
	Serial.print("AP IP address: ");
	Serial.println(myIP);
        Serial.print("ADC VALUE: "+String(raw));
        //Serial.println(raw);
	server.on("/", handleRoot);
	server.begin();
	Serial.println("HTTP server started");
}
//=======================================================================
//                MAIN LOOP
//=======================================================================
void loop()
{
  server.handleClient();
}
//================================
