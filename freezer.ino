#include <GyverSegment.h> // библиотека для TM1637
#include <TimerMs.h> // библиотека для создания таймеров

// объявление таймеров (мс, запущен/нет, период/таймер)
TimerMs tmr05(500, 1, 0);
TimerMs tmr1(1000, 1, 0);
TimerMs tmr10(10000, 0, 1);

// пины индикатора
#define CLK_PIN 8
#define DIO_PIN 9

#define HYR_PIN 3 // пин гиркона

#define LED_PIN 4 // пин светодиода

#define BUZZ_PIN 12 // пин пищалки

bool dots = false; // состояние точек индикатора
int openCounter = 0; // счётчик открытий
int secCounter = 0; // счётчик секунд
bool IsWarning = false; // предупреждение о долгом открытии
bool IsOpened = false; // предыдущее состояние дверцы

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
    
    if (IsWarning){ // мигание светодиодом при долгом открытии
      digitalWrite(LED_PIN, dots);
    }
  }

  if (tmr1.tick()){
    bool hyrStat = digitalRead(HYR_PIN); // получение состояния гиркона

    // обновление состояния на открытый
    if (hyrStat){
      if (!IsOpened){
        IsOpened = true;

        dosignal(1); // сигнал открытия

        secCounter = 0;
        tmr10.start();

        openCounter++;

        disp.showClock(openCounter, secCounter);
        digitalWrite(LED_PIN, 1);
      }

      // обновление секундомера
      secCounter++;
      disp.showClock(openCounter, secCounter);
    }

    // обновление состояния на закрытый
    else {
      if (IsOpened){
        IsWarning = false;
        IsOpened = false;

        dosignal(3); // сигнал закрытия

        tmr10.stop(); // остановка таймера предупреждения

        digitalWrite(LED_PIN, 0);
        disp.clear();
        disp.update();
      }
    }
  }

  // активация режима предупреждения при долгом открытии
  if (tmr10.tick()){
    IsWarning = true;

    dosignal(2); // сигнал предупреждения

    tmr10.stop(); // остановка таймера предупреждения
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
      ftone(300, 100);
      ftone(100, 500);
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
