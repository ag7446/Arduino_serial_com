#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
 
// Replace with your network credentials
const char* ssid = "Telia-55EA1D";  // Enter SSID here
const char* password = "9F2605B50B";  //Enter Password here
 
ESP8266WebServer server(80);   //instantiate server at port 80 (http port)
 
String page = "";
//double data; 
//int timer[3];
//int timerCount = 0;
int time_tot;
int sec;
int Min;
int hrs;
void setup(void){
 //timerIndex = 0;
  //pinMode(A0, INPUT);
  
  delay(1000);
  Serial.begin(115200);
  WiFi.begin(ssid, password); //begin WiFi connection
  Serial.println("");
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/", [](){
    page = "<meta http-equiv=\"refresh\" content=\"0.5\"><h1>Smart Timer</h1><h3>Time:</h3> <h4>"+String(hrs) + ":" + String(Min) + ":" + String(sec)+"</h4>";
    server.send(200, "text/html", page);
     
  });
  
  server.begin();
  Serial.println("Web server started!");
}
 
void loop(void){
  
if(Serial.available()>0)
{
    sec = Serial.read();
    Serial.write(1);
    while(Serial.available() <= 0) 
    {}
    Min = Serial.read();
    Serial.write(1);
    while(Serial.available() <= 0) 
    {}
    hrs = Serial.read();

    Serial.println(sec);
    Serial.println(Min);
    Serial.println(hrs);
}
  server.handleClient();
}
