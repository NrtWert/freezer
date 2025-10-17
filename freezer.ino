
#include <GyverTM1637.h>
#include <TimerMs.h>

TimerMs tmr025(250, 1, 0);
TimerMs tmr05(500, 1, 0);
TimerMs tmr1(1000, 1, 0);
TimerMs tmr10(10000, 1, 1);

const int buttonPin = 2;  // the number of the pushbutton pin
const int ledPin = 4;    // the number of the LED pin
const int piezoPin = 12;
const int dispclkPin = 8;
const int dispdioPin = 9;

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status

int iOpenCounter = 0;
int iSecCounter = 0;
bool bIsWarning = false;
bool bIsOpened = false;

uint32_t Now, clocktimer;
bool flag = false;

GyverTM1637 disp(dispclkPin, dispdioPin);


void setup() {
  disp.clear();
  disp.brightness(7);  // яркость, 0 - 7 (минимум - максимум)
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  dosignal(0);
}

void loop() {
  if (tmr05.tick()) {
    flag = !flag;
    disp.point(flag);   // выкл/выкл точки
  }
  if (tmr1.tick()) {
    buttonState = digitalRead(buttonPin);
    if (buttonState == HIGH) {
      // freezer closed:
      if (bIsOpened){
        dosignal(3);
        bIsWarning = false;
      }
      bIsOpened = false;
      digitalWrite(ledPin, LOW);
    } 
    else {
      // freezer opened:
      if (!bIsOpened){
        iSecCounter = 0;
        dosignal(1);
        iOpenCounter++;
      }
      iSecCounter++;
      bIsOpened = true;
      showondisplay(iOpenCounter,iSecCounter);
      digitalWrite(ledPin, HIGH);
    }


    
  };

  if (tmr10.tick()) {
    bIsWarning = true;
    dosignal(2);
  }
}

void showondisplay(uint8_t left2dig,uint8_t right2dig){
  if (left2dig > 99) left2dig = 99;
  if (right2dig > 99) right2dig = 99;
  disp.clear();
  disp.displayClock(left2dig,right2dig);
}

void dosignal(byte sygnaltype){
  // 0 - START SIGNAL
  // 1 - OPEN SIGNAL
  // 2 - WARN SIGNAL
  // 3 - CLOSE SIGNAL
  switch (sygnaltype) {
    case 0:
      tone(piezoPin, 1000, 1000);
      delay(1000);
      noTone(piezoPin);
      break;
    case 1:
      tone(piezoPin, 600);
      delay(100);
      tone(piezoPin, 800);
      delay(500);
      noTone(piezoPin);
      break;
    case 2:
      tone(piezoPin, 300);
      delay(100);
      tone(piezoPin, 100);
      delay(500);
      noTone(piezoPin);
      break;
    case 3:
      tone(piezoPin, 1000, 200);
      delay(200);
      tone(piezoPin, 1300, 200);
      delay(200);
      tone(piezoPin, 1600, 200);
      delay(200);
      tone(piezoPin, 1300, 200);
      delay(200);
      tone(piezoPin, 1000, 200);
      delay(200);
      noTone(piezoPin);
      break;
    default:
      break;
  }
}

void twistClock() {
  byte hrs = 21, mins = 55;
  uint32_t tmr;
  Now = millis();
  while (millis () - Now < 10000) {   // каждые 10 секунд
    if (millis() - tmr > 1000) {       // каждые полсекунды
      tmr = millis();
      flag = !flag;
      disp.point(flag);   // выкл/выкл точки

      if (flag) {
        // ***** часы! ****
        mins ++;
        if (mins > 59) {
          mins = 0;
          hrs++;
          if (hrs > 24) hrs = 0;
        }
        // ***** часы! ****
        disp.displayClockTwist(hrs, mins, 35);    // выводим время
      }
    }
  }
  disp.point(0);   // выкл точки
}
