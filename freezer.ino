#include <GyverSegment.h> // библиотека для TM1637
#include <TimerMs.h> // библиотека для создания таймеров

// объявление таймеров (мс, запущен/нет, период/таймер)
TimerMs tmr025(500, 0, 0);
TimerMs tmr05(500, 1, 0);
TimerMs tmr1(1000, 1, 0);

// пины индикатора
#define CLK_PIN 8
#define DIO_PIN 9

#define HYR_PIN 3 // пин гиркона

#define LED_PIN 4 // пин светодиода

#define BUZZ_PIN 12 // пин пищалки

bool dots = false; // состояние точек индикатора
short Warning = 0; // предупреждение о долгом открытии
bool IsOpened = false; // предыдущее состояние дверцы
int secOnDay = 0; // счёчик секунд за день
int openCounter = 0; // счётчик открытий
int secCounter = 0; // счётчик секунд

Disp1637Colon disp(DIO_PIN, CLK_PIN); // идентификация индикатора

void setup() {
  disp.clear();
  disp.brightness(7);  // яркость, 0 - 7 (минимум - максимум)

  pinMode(HYR_PIN, INPUT_PULLUP); // подтягивание гиркона через втроенный резистор
  pinMode(LED_PIN, OUTPUT); // пин светодиода на выход

  dosignal(0); // сигнал запуска

  Serial.begin(9600);
}

void loop() {
  if (tmr05.tick() and IsOpened){ // мигание точками на индикаторе
    dots = !dots;
    disp.colon(dots);
    
    if (Warning >= 1){ // мигание светодиодом при долгом открытии
      digitalWrite(LED_PIN, dots);
      if (Warning == 2){
        ftone(1000, 200);
      }
      if (Warning == 3){
        int x = 0;
        if (dots){x = 11;}
        disp.showClock(openCounter, x);
      }
    }
  }

  if (tmr025.tick()){
    ftone(1300, 100);
  }

  if (tmr1.tick()){
    bool hyrStat = digitalRead(HYR_PIN); // получение состояния гиркона

    // обновление состояния на открытый
    if (hyrStat){
      if (!IsOpened){
        IsOpened = true;

        dosignal(1); // сигнал открытия

        disp.showClock(secCounter / 60, secCounter % 60);
        secCounter = 0;

        openCounter++;

        digitalWrite(LED_PIN, 1);
      }

      // обновление секундомера
      secCounter++;
      
      if (secCounter > 4 and secCounter < 8){
        disp.showClock(secOnDay / 60, secOnDay % 60);
      }
      else if (secCounter > 7){
        if (secCounter <= 90){
          disp.showClock(openCounter, secCounter);
        }
        else {
          Warning = 3;
          tmr025.start();
        }
      }

      if (secCounter == 30){
        dosignal(2);
        Warning = 1;
      }
      else if (secCounter == 60){
        Warning = 2;
      }
    }

    // обновление состояния на закрытый
    else {
      if (IsOpened){
        Warning = 0;
        IsOpened = false;

        secOnDay += secCounter; // обновление счётчика за день

        dosignal(3); // сигнал закрытия

        tmr025.stop();

        digitalWrite(LED_PIN, 0);
        disp.clear();
        disp.update();
      }
    }
  }
}

// сокращение функции dosignal
void ftone(int hz, int t){
  tone(BUZZ_PIN, hz, t);
  delay(t);
}

// функция для воспроизведения сигналов для оповещения
void dosignal(byte sygnaltype){
  // 0 - START SIGNAL
  // 1 - OPEN SIGNAL
  // 2 - WARN SIGNAL
  // 3 - CLOSE SIGNAL
  switch (sygnaltype) {
    case 0:
      ftone(1000, 1000);
      break;
    case 1:
      ftone(600, 100);
      ftone(800, 500);
      break;
    case 2:
      ftone(1000, 400);
      ftone(1300, 500);
      break;
    case 3:
      ftone(1000, 200);
      ftone(1300, 200);
      ftone(1600, 200);
      ftone(1300, 200);
      ftone(1000, 200);
      break;
    default:
      break;
  }

  noTone(BUZZ_PIN);
}
