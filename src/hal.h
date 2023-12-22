#ifndef __HAL_H
#define __HAL_H

#include <MPU6050_tockn.h>
#include <Arduino.h>
#include <U8g2lib.h> //调用u8g2庿
#include "CommonMacro.h"
#include <extEEPROM.h>
#include "servo.hpp"
#include "wheel.hpp"
#include "power.h"
#include <LED.h>
#include <EEPROM.h> //stm32 内部EEPROM
#include <Button.h>
// #include <Preferences.h>
// #include <SSD1306Wire.h>
// #include <WiFi.h>

#define SDA2_PIN 19
#define SCL2_PIN 18

#define RX1_PIN 27
#define TX1_PIN 12

#define LED_PIN PA12

#define ADC_PIN PB1

#define PWM3 PA6
#define PWM4 PA7

#define T3_PIN 15

#define DEBUG

#define __digitalFlip(pin) digitalWrite(pin, !digitalRead(pin))

enum Dir_t
{
  LEFT = 1,
  RIGHT
};

/* IMU */
typedef struct
{
  float ax;
  float ay;
  float az;
  float gx;
  float gy;
  float gz;
  float mx;
  float my;
  float mz;
  float roll;
  float yaw;
  float pitch;
} IMU_Info_t;

// serial
void serial_init();

// led
void led_init();
void led_update();

// power
extern Power power;

// serial
extern HardwareSerial Serial2;

// wheel
extern Motor motorL;
extern Motor motorR;
extern Wheel wheelL;
extern Wheel wheelR;

// imu
extern MPU6050 mpu6050;
void imu_init();
void imu_update();
void imu_calc();

// ps4
void ps4_init();
void ps4_update();

// oled
// extern SSD1306Wire display;
extern U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2;
void oled_init();
void oled_update();

// wifi
#ifdef DEBUG
extern bool wifiConnectFlag2;
// extern WiFiClient client2;
#endif
void wifi_init();
void wifi_update();

// AT24C04
extern extEEPROM eep;

// hal
void hal_init();
void hal_update();

// servo
extern Servo srvL;
extern Servo srvR;

#endif
