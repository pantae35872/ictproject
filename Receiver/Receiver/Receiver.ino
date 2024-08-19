#include <IRremote.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

decode_results results;


enum class Command {
  Open = 0x33,
  Close = 0x34,
};

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(5, OUTPUT);
  IrReceiver.begin(2, DISABLE_LED_FEEDBACK);
}

void wakeUp() {
}

void sleepNow()
{
  sleep_enable();
  attachInterrupt(digitalPinToInterrupt(2), wakeUp, RISING);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_mode();
  sleep_disable();
  detachInterrupt(digitalPinToInterrupt(2));
}

void loop() {
  Serial.end();
  sleepNow();
  Serial.begin(115200);
  delay(200);

  if (IrReceiver.decode()) {
    IrReceiver.resume();
    Serial.println(IrReceiver.decodedIRData.command, HEX);
    Command command = (Command)IrReceiver.decodedIRData.command;
    if (command == Command::Open) {
      digitalWrite(5, HIGH);
      delay(5000);
      digitalWrite(5, LOW);
    }
  }
}