// DS3231 Alarm Clcok From Hell

#include <Wire.h>
#include "DS3231.h"
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#define I2C_ADDRESS_OLED 0x3C
#define PIN_BUZZER 10

#define ALARM_TOGGLE_DURATION 500

// inclusive start time
#define ALARM_START_HOUR    8
#define ALARM_START_MINUTE  0

// inclusive end time
#define ALARM_END_HOUR    11
#define ALARM_END_MINUTE  0

#define BUZZER_DUTY_ON 96
#define BUZZER_DUTY_OFF 255

DateTime now;
RTClib RTC;

SSD1306AsciiWire oled;

void setup () {
  pinMode(PIN_BUZZER, OUTPUT);
  Serial.begin(9600);
  Wire.begin();
  oled.begin(&Adafruit128x64, I2C_ADDRESS_OLED);
  oled.setFont(fixed_bold10x15);
  oled.set400kHz();
  oled.clear();

}

void loop () {
  static uint8_t last_second = 255;
  static uint32_t last_millis = millis();
  static uint32_t last_elapsed = 0;
  static uint32_t elapsed = 255;
  int hour_value;
  //  digitalWrite(PIN_BUZZER, !digitalRead(PIN_BUZZER));

  now = RTC.now();
  if (now.second() != last_second) {
    last_millis = millis();
    last_second = now.second();
    oled.home();
    oled.print(now.year(), DEC);
    oled.print('-');
    if (now.month() < 10)
      oled.print('0');
    oled.print(now.month(), DEC);
    oled.print('-');
    oled.print(now.day(), DEC);
    oled.println("                    ");
    hour_value = now.hour() ;
    if (hour_value > 11) {
      hour_value -= 12;
    }
    if (hour_value  == 0)   {
      oled.print(12, DEC );
    } else {
      if (hour_value < 10) {
        oled.print(' ');
      }
      oled.print(hour_value, DEC);
    }

    oled.print(':');
    if (now.minute() < 10) {
      oled.print('0');
    }

    oled.print(now.minute(), DEC);
    oled.print(':');

    if (now.second() < 10) oled.print('0');

    oled.print(now.second(), DEC);

    if (now.hour() > 11) oled.print("PM");
    else oled.print("AM");

    oled.println("                    ");
  }
  elapsed = constrain(millis() - last_millis, 0, 1000);
  elapsed = map(elapsed, 0, 1000, 0 , 11);
  if (last_elapsed != elapsed) {
    oled.setCursor(0, 45);
    for (int i = 0; i < elapsed; i++) {
      oled.print("-");
    }
    //    for (int i = 0; i < elapsed; i++) {
    //      oled.print(" ");
    //    }
    //    oled.print("-");
    oled.clearToEOL();
  }

  process_alarm();
}

void process_alarm() {
  static bool buzz_state = false;
  static uint32_t buzz_state_millis = 0;
  static int duty_cycle = BUZZER_DUTY_OFF;

  int alarm_start_time = ALARM_START_HOUR * 60 + ALARM_START_MINUTE;
  int alarm_end_time = ALARM_END_HOUR * 60 + ALARM_END_MINUTE;
  int time_current = now.hour() * 60 + now.minute();

  //  if (
  //    (now.hour() >= ALARM_START_HOUR && now.minute() >= ALARM_START_MINUTE) &&
  //    (now.hour() <= ALARM_END_HOUR && now.minute() <= ALARM_END_MINUTE)
  //  ) {
  // TODO add logic to handle an earlier end time than start time
  if (time_current >= alarm_start_time && time_current <= alarm_end_time) {
    if (millis() - buzz_state_millis > ALARM_TOGGLE_DURATION) {
      buzz_state_millis = millis();
      if (duty_cycle == BUZZER_DUTY_OFF) {
        duty_cycle = BUZZER_DUTY_ON;
      } else {
        duty_cycle = BUZZER_DUTY_OFF;
      }
    }
  } else {
    duty_cycle = BUZZER_DUTY_OFF;
  }
  analogWrite(PIN_BUZZER, duty_cycle);
}
