#include <IRremote.hpp>
#include <Wire.h>
#include <SPI.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

//IRsend IrSend;

DateTime last_date_time(SECONDS_FROM_1970_TO_2000);

enum class Mode {
  Auto,
  Manaul
};

enum class Command {
  Open = 0x33,
  Close = 0x34,
};

RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 20, 4);
bool alarm = false;
Mode current_mode = Mode::Manaul;
#define CLOCK_INTERRUPT_PIN 2

void setup() {
  pinMode(4, INPUT);
  IrSender.begin(3);
  lcd.init();
  lcd.backlight();
  lcd.display();

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }


  rtc.disable32K();

  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  rtc.writeSqwPinMode(DS3231_OFF);
    
  rtc.disableAlarm(2);

  if(!rtc.setAlarm1(
            rtc.now() + TimeSpan(10),
            DS3231_A1_Minute // this mode triggers the alarm when the seconds match. See Doxygen for other options
  )) {
    //Serial.println("Error, alarm wasn't set!");
  }else {
    //Serial.println("Alarm will happen in 10 seconds!");
  }
  
  pinMode(CLOCK_INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CLOCK_INTERRUPT_PIN), onAlarm, FALLING);
}

void onAlarm() {
  alarm = true;
}

void displayStatus() {
  DateTime now = rtc.now();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mode: ");
  switch (current_mode) {
    case Mode::Manaul:
      lcd.print("Manaul");
      break;
    case Mode::Auto:
      lcd.print("Auto");
      break;
  }
  lcd.setCursor(0, 1);
  lcd.print("Current Time: ");
  lcd.setCursor(0, 2);
  lcd.print(now.hour(), DEC);
  lcd.print(':');
  String printminute = String(now.minute());
  if (printminute.length() < 2) {
    lcd.print("0");
  }
  lcd.print(now.minute(), DEC);
  lcd.print(':');
  String printsecond = String(now.second());
  if (printsecond.length() < 2) {
    lcd.print("0");
  }
  lcd.print(now.second(), DEC);
  if (alarm) {
    lcd.setCursor(0, 3);
    lcd.print("Alarm!!");
    rtc.clearAlarm(1);
    alarm = false;
  }
}

void send(Command command) {
  IrSender.mark(500);
  IrSender.space(0);
  delay(100);
  IrSender.sendNEC(0xFA, (uint8_t)command, 0);
}

void loop() {
  if (last_date_time != rtc.now() || alarm) {
    displayStatus();
  }
  if (digitalRead(4) == LOW) {
    send(Command::Open);
  }
  last_date_time = rtc.now();
  delay(10);
  //IrSend.mark(500);
  //IrSend.space(0);
  //delay(1000);
  //IrSend.sendSony(0xFFF, 12);
  //delay(2000);
  //IrSend.sendSony(0xFF0, 12);
  //delay(2000);
}