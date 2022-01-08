#if 1

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#define MinPRESSURE 200
#define MAXPRESSURE 1000

#define CLK 22
#define DT 24
#define SW 26


const int XP=7,XM=A1,YP=A2,YM=6; //240x320 ID=0x1526
const int TS_LEFT=908,TS_RT=208,TS_TOP=929,TS_BOT=193; //re-calibrated for the screen at hand 
const int buzzer = 31;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Adafruit_GFX_Button start_btn, reset_btn, fifteen_btn, five_btn, thirty_btn;

int pixel_x, pixel_y;     //Touch_getXY() updates global vars
bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MinPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
         pixel_x = map(p.y, TS_LEFT, TS_RT, 0, 320); // Specifying screen layout and creating a map
         pixel_y = map(p.x, TS_TOP, TS_BOT, 0, 240);
    }
    return pressed;
}

int hrs = 0;
int Min = 0;
int sec = 0;
bool countdown = false;
int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir ="";
unsigned long lastButtonPress = 0;



#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

unsigned long startMillis;  //Start value on millis (internal clock that sarts upon power on)
unsigned long currentMillis; // current value since start
unsigned long benchMillis;

void setup() {

  
  // Set up section for program to run:
 // uint16_t ID = tft.readID();
    // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(115200);
  //Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial3.begin(115200);

  tft.begin(0x9341);
  tft.fillScreen(BLACK);
  tft.setRotation(3);
  start_btn.initButton(&tft,  210, 190 , 80, 70, WHITE, CYAN, BLACK, "||>", 2);
  reset_btn.initButton(&tft, 110 , 190 , 80, 70, WHITE, CYAN, BLACK, "Reset", 2);
  start_btn.drawButton(false);
  reset_btn.drawButton(false);

  five_btn.initButton(&tft, 270, 40, 80, 70, WHITE, BLACK, GREEN, "+5Min", 2);
  fifteen_btn.initButton(&tft,  160, 40, 80, 70, WHITE, BLACK, RED, "+15Min", 2);
  fifteen_btn.drawButton(false);
  five_btn.drawButton(false);

  thirty_btn.initButton(&tft, 50, 40, 80, 70, WHITE, BLACK, GREEN, "+30Min", 2);
  thirty_btn.drawButton(false);  


  pinMode(CLK,INPUT);
  pinMode(DT,INPUT);
  pinMode(SW, INPUT_PULLUP);

  // Read the initial state of CLK
  lastStateCLK = digitalRead(CLK);
  pinMode(buzzer, OUTPUT);
}

void loop(void)
{
    bool down = Touch_getXY();
    start_btn.press(down && start_btn.contains(pixel_x, pixel_y));
    reset_btn.press(down && reset_btn.contains(pixel_x, pixel_y));
    five_btn.press(down && five_btn.contains(pixel_x, pixel_y));
    fifteen_btn.press(down && fifteen_btn.contains(pixel_x, pixel_y));
    thirty_btn.press(down && thirty_btn.contains(pixel_x, pixel_y));

    if (start_btn.justPressed() && countdown == true) {
        start_btn.drawButton(true);
        startMillis = millis();
        countdown = false;
        start_btn.drawButton(false);  
    } else if (start_btn.justPressed() && countdown == false) {
        start_btn.drawButton(true);
        startMillis = millis();
        countdown = true;
        start_btn.drawButton(false); 
    }
    
    if (reset_btn.justPressed()) {
        reset_btn.drawButton(true);
        startMillis = millis();
        tft.fillRect(70, 90, 210, 50, BLACK);
        sec = 0;
        Min = 0;
        hrs = 0;
        countdown = false;
        reset_btn.drawButton(false); 
    }

    if (five_btn.justPressed()) { // button for adding 5 Min
        five_btn.drawButton(true);
        tft.fillRect(70, 90, 210, 50, BLACK);
        Min = Min + 5;
        if (Min >= 60) {
          hrs = hrs + 1;
          Min = Min - 60;
        }
        five_btn.drawButton(false);   
     }
     
     if (fifteen_btn.justPressed()) { // button for adding 15 Min
        fifteen_btn.drawButton(true);
        tft.fillRect(70, 90, 210, 50, BLACK);
        Min = Min + 15;
        if (Min >= 60) {
          hrs = hrs + 1;
          Min = Min - 60;
        }
        fifteen_btn.drawButton(false);  
     } 
     
     if (thirty_btn.justPressed()) { // button for adding 30 Min
        thirty_btn.drawButton(true);
        tft.fillRect(70, 90, 210, 50, BLACK);
        Min = Min + 30;
        if (Min >= 60) {
          hrs = hrs + 1;
          Min = Min - 60;
        }
        thirty_btn.drawButton(false);   
     }
      
     lastStateCLK = currentStateCLK;

     // Read the button state
      int btnState = digitalRead(SW);
    
      //If we detect LOW signal, button is pressed
      if (btnState == LOW) {
        
        //if 100ms have passed since last LOW pulse, it means that the
        //button has been pressed, released and pressed again
        //if value is lower (50ms) the button and loop registers 2 clicks...
        if (millis() - lastButtonPress > 100) {
          countdown = !countdown;
        }
      
        // Remember last button press event
        lastButtonPress = millis();
      }
      if (Min == 0 && hrs == 0 && sec == 0) { //stop statement for timer
           countdown = false;
      }

      // Put in a slight delay to help debounce the reading
      delay(1);

    currentStateCLK = digitalRead(CLK);

      // If last and current state of CLK are different, then pulse occurred
      // React to only 1 state change to avoid double count
      if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){
    
        // If the DT state is different than the CLK state then
        // the encoder is rotating CCW so decrement
        if (digitalRead(DT) != currentStateCLK) {
          counter ++;
          currentDir ="CW";
        } else {
          // Encoder is rotating CW so increment
          counter --;
          currentDir ="CCW";
        }
    
        Serial.print("Direction: ");
        Serial.print(currentDir);
        Serial.print(" | Counter: ");
        Serial.println(counter);

        if (currentDir == "CW") {
          tft.fillRect(70, 90, 210, 50, BLACK);
          if (sec+10 < 60) {
                sec = sec + 10;
            } else if ( sec + 10 >= 60) {
                Min = Min + 1;
                if (Min >= 60) {
                  hrs = hrs + 1;
                  Min = Min - 60;
                }
                sec = sec + 10 - 60; 
             } 
        }

       if (currentDir == "CCW") {
          tft.fillRect(70, 90, 210, 50, BLACK);
          if(sec-10 < 0 && Min == 0 && hrs == 0) {
                sec = 0;
             }else if (sec-10 >= 0) {
                sec = sec - 10;
            } else if ( sec - 10 < 0) {
                Min = Min - 1;
                sec = sec  - 10 + 60;
                if (Min < 0) {
                  hrs = hrs - 1;
                  Min = Min + 60;
                }
             }  
          }
      }
           
      if (sec >= 0 && countdown == true) {
          currentMillis = millis();
          if (currentMillis-startMillis >= 1000) { //sampel time for update rate
              sec = sec -1;
              if (sec < 0) {
                  Min = Min - 1;
                  sec = sec + 60;
                  if (Min < 0) {
                      hrs = hrs - 1;
                      Min = Min + 60;
                  }
              }

              if (Min == 0 && hrs == 0 && sec == 0) { //stop statement for timer
                  countdown = false;
                  tft.fillRect(70, 90, 210, 50, BLACK); //print animation for timer being done
                  tft.setCursor(70, 90);
                  tft.setTextSize(6);
                  tft.setTextColor(GREEN); 
                  tft.print("DONE");
                  tone(buzzer, 1000); // Send 1KHz sound signal...
                  delay(500);        // ...for .5 sec
                  noTone(buzzer);     // Stop sound...
                  delay(500);
                  tone(buzzer, 1000); // Send 1KHz sound signal...
                  delay(500);        // ...for .5 sec
                  noTone(buzzer);     // Stop sound...
                  delay(500);
                  tone(buzzer, 1000); // Send 1KHz sound signal...
                  delay(500);        // ...for .5 sec
                  noTone(buzzer);     // Stop sound...
              }
              tft.fillRect(70, 90, 210, 50, BLACK);
              tft.setCursor(70, 90);
              tft.setTextSize(5);
              tft.setTextColor(WHITE);
              tft.print(String(hrs) + ":" + String(Min) + ":" + String(sec));
              startMillis = currentMillis;
          }        
      }
    
      tft.setCursor(70, 90);
      tft.setTextSize(5);
      tft.setTextColor(WHITE); 
      tft.print(String(hrs) + ":" + (Min) + ":" + String(sec));
   
      if (Serial3.available()>0) {
          Serial3.write(sec);
          while(Serial3.available() <= 0) 
          {}
          Serial3.write(Min);
          while(Serial3.available() <= 0) 
          {}
          Serial3.write(hrs);
      }
}
#endif
