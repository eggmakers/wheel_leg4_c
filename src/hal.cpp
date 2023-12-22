#include "hal.h"

// led
LED led(LED_PIN);
void led_init()
{
  led.begin(1000);
}
void led_update()
{
  led.blink(0, 1000);
}
// power
Power power(ADC_PIN);
void power_init()
{
  power.bigen();
}
// oled
#define I2C1_SDA PB7
#define I2C1_SCL PB6
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/I2C1_SCL, /* data=*/I2C1_SDA, /* reset=*/U8X8_PIN_NONE); // 使用软件IIC,SCL PB6,SDA PB7
void oled_init()
{
  u8g2.begin(); // 初始化显示屏
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_spleen5x8_mf); //(u8g2_font_ncenR12_tr); // 选择12*12字体（具体字体说明可以在git上搜索u8g2查看）
  Serial.println("oled_init");
}
void oled_update()
{
}
// imu
TwoWire Wire3; //(PC_9,PA_8);
MPU6050 mpu6050(Wire3);
void imu_init()
{
  Wire3.begin((uint32_t)PC9, (uint32_t)PA8);
  mpu6050.setGyroOffsets(-2.31, 0.13, 1.60);
  mpu6050.begin();
  Serial.println("mpu6050 imu_init");
}
void imu_update()
{
  mpu6050.update();
}
void imu_calc()
{
  mpu6050.calcGyroOffsets(true);
}
// AT24CXX
TwoWire Wire2(PB11, PB10);
extEEPROM eep(kbits_4, 1, 16, 0x50);
void at24c_init()
{
  uint8_t eepStatus;
  eepStatus = eep.begin(eep.twiClock400kHz, &Wire2); // go fast!
  if (eepStatus)
  {
    Serial.print(F("extEEPROM.begin() failed, status = "));
    Serial.println(eepStatus);
  }
}
// wheel
HardwareSerial Serial3(PD9, PD8);
HardwareSerial Serial6(PC7, PC6);
float Motor::deadzone = 0;
Motor motorL(Serial6);
Motor motorR(Serial3);
Wheel wheelL(motorL, true);
Wheel wheelR(motorR, false);
void wheel_init()
{
  Serial3.begin(115200);
  Serial6.begin(115200);
  motorL.begin(true);
  motorR.begin(true);
}
void wheel_update()
{
  motorL.update();
  motorR.update();
}
// serial
HardwareSerial Serial1(PA10, PA9);
HardwareSerial Serial2(PA3, PA2);
void serial_init()
{
  Serial.begin(115200);
  Serial2.begin(115200); // ESP32

  Serial.println("serial_init");
}
// servo
Servo srvL(PWM3);
Servo srvR(PWM4);
void servo_init()
{
  Servo::begin();
  // srvL.attach(90);
  // srvR.attach(0);
  // srvL.write(180,30);
  // srvR.write(180,30);
}
void servo_update()
{
  srvL.update();
  srvR.update();
}

void debug_update();
void hal_init()
{
  delay(500); // 等待上电稳定
  serial_init();
  led_init();
  imu_init();
  oled_init();

  servo_init();
  // Serial.printf("srvl angle: %.3f , plus: %d us",srvL.read(),srvL.readMicroseconds());
  // Serial.print("srvl angle:");Serial.print(srvL.read());Serial.print(" plus:");Serial.println(srvL.readMicroseconds());
  // srvL.attach(45);
  // Serial.print("srvl angle:");Serial.print(srvL.read());Serial.print(" plus:");Serial.println(srvL.readMicroseconds());
  // pwm_init();
#ifdef DEBUG
  // wifi_init();
#else
  ps4_init();
#endif
  power_init();
  at24c_init();
  wheel_init();
  // motor_init();
#if 0
  legL.set(90,30);
  legR.set(90,30);
  wheelL.setTorque(1,false);
  wheelR.setTorque(1,false);
#endif
}

void hal_update()
{
  led_update();
  imu_update();
  // motor_update();
  // oled_update();
  servo_update();
  wheel_update();
  // button_update();
#ifdef DEBUG
  // wifi_update();
#else
  ps4_update();
#endif
  debug_update();
}

void debug_update()
{
#if 0   // 验证调试串口1、运行指示灯led
  Serial.println("hello1");
  delay(200);
  digitalWrite(PA12,HIGH);
  delay(200);
  digitalWrite(PA12,LOW);
#elif 0 // 验证无线wifi和蓝牙ps4手柄 串口2
  int size = Serial2.available();
  if (size)
  {
    uint8_t buf[64];
    size = Serial2.readBytes(buf, size);
    Serial2.write(buf, size);
  }
#elif 0 // 验证串口3/6,foc和电机通讯
  int size = Serial6.available();
  if (size)
  {
    uint8_t buf[64];
    size = Serial6.readBytes(buf, size);
    Serial.write(buf, size);
  }
  size = Serial3.available();
  if (size)
  {
    uint8_t buf[64];
    size = Serial3.readBytes(buf, size);
    Serial.write(buf, size);
  }
  size = Serial.available();
  if (size)
  {
    uint8_t buf[64];
    size = Serial.readBytes(buf, size);
    Serial3.write(buf, size);
    Serial6.write(buf, size);
  }
#elif 0
  static long timer = 0;
  // Serial.println("2");
  mpu6050.update();

  if (millis() - timer > 200)
  {
    Serial.print("angleX : ");
    Serial.print(mpu6050.getAngleX());
    Serial.print("\tangleY : ");
    Serial.print(mpu6050.getAngleY());
    Serial.print("\tangleZ : ");
    Serial.println(mpu6050.getAngleZ());
    timer = millis();
  }
#elif 0
  static long int t = 0;
  // u8g2.begin();               // 初始化显示屏
  u8g2.clearBuffer();                    // 清屏
  u8g2.setFont(u8g2_font_spleen8x16_mf); //(u8g2_font_ncenR12_tr); // 选择12*12字体（具体字体说明可以在git上搜索u8g2查看＿
  u8g2.drawStr(0, 16, "Hello World!");   // 注意这里昿0,0开始显示Hello World
  // u8g2.drawStr(0, 32, "Hello!"); // 注意这里昿0,16开始显示Hello＿
  // u8g2.drawStr(0, 48, "Hi!"); // 注意这里昿0,32开始显示Hello＿
  u8g2.setCursor(0, 32); // 设置打印起始地址（下面的t显示在什么地方）
  u8g2.print(t);         // 显示t
  // u8g2.drawStr(48, 64, "C"); // write something to the internal memory
  // u8g2.drawStr(112, 64, "%"); // write something to the internal memory

  u8g2.sendBuffer(); // 将以上所有的缓存内容显示
  Serial.println(t); // 串口输出t＿
  delay(500);        // 延时0.5秿
  t++;               // 为了区别,t自加1
  /*  for (size_t i = 64; i < 288; i++)
   {
     u8g2.setFont(u8g2_font_tenstamps_mn);
     u8g2.setCursor(0, 20);
     u8g2.print("   ");//为了不全屏刷新的情况下清空局部显示区埿
     u8g2.setCursor(0, 20);
     u8g2.print(i);
     u8g2.setFont(u8g2_font_inr33_mf);
     u8g2.setCursor(64, 32);
     u8g2.print("   ");//为了不全屏刷新的情况下清空局部显示区埿
     u8g2.setFont(u8g2_font_open_iconic_all_4x_t);
     u8g2.drawGlyph(64, 32, i);
     u8g2.sendBuffer();
     delay(1500);
   } */

#elif 0
  Serial.print(analogRead(ADC_PIN));
  Serial.print("  ");
  Serial.print(power_get_voltage());
  Serial.print("  ");
  Serial.println(power_get());
  delay(10);
#elif 0
  u8g2.clearBuffer();                    // 清屏
  u8g2.setFont(u8g2_font_spleen8x16_mf); //(u8g2_font_ncenR12_tr); // 选择12*12字体（具体字体说明可以在git上搜索u8g2查看＿
  u8g2.drawStr(0, 16, "Hello World!");   // 注意这里开始显示Hello World
  u8g2.setCursor(0, 32);                 // 设置打印起始地址（下面的t显示在什么地方）
  int quant = power.getQuantity(0.01);
  u8g2.print(quant); // 显示t
  u8g2.sendBuffer(); // 将以上所有的缓存内容显示
  delay(20);         // 延时0.5秿
  Serial.print(" AdcDigit: ");
  Serial.print(power.getAdcDigit());
  Serial.print(" FilterAdcDigit: ");
  Serial.print(power.getFilterAdcDigit());
  Serial.print(", AdcDigit: ");
  Serial.print(power.getAdcAnalog());
  Serial.print(", PowerVoltage: ");
  Serial.print(power.getPowerVoltage());
  Serial.print(", FilterVoltage: ");
  Serial.print(power.getFilterVoltage());
  Serial.print(", OneVoltage: ");
  Serial.print(power.getOneVoltage());
  Serial.print(", Quantity: ");
  Serial.println(quant);
#elif 0
  u8g2.clearBuffer();                    // 清屏
  u8g2.setFont(u8g2_font_spleen8x16_mf); //(u8g2_font_ncenR12_tr); // 选择12*12字体（具体字体说明可以在git上搜索u8g2查看＿
  u8g2.drawStr(0, 16, "Hello World!");   // 注意这里开始显示Hello World
  u8g2.setCursor(0, 32);                 // 设置打印起始地址（下面的t显示在什么地方）
  int quant = power.getFilterQuantity();
  u8g2.print(quant); // 显示t
  u8g2.sendBuffer(); // 将以上所有的缓存内容显示
  delay(20);         // 延时0.5秒
  Serial.print(" AdcDigit: ");
  Serial.print(power.getAdcDigit());
  Serial.print(", AdcDigit: ");
  Serial.print(power.getAdcAnalog());
  Serial.print(", PowerVoltage: ");
  Serial.print(power.getPowerVoltage());
  Serial.print(", OneVoltage: ");
  Serial.print(power.getOneVoltage());
  Serial.print(", Quantity: ");
  Serial.print(power.getQuantity());
  Serial.print(", FilterQuantity: ");
  Serial.println(quant);
#elif 0
  Serial.print("L");
  Serial.println(wheelL.getSpeed());
  Serial.print("R");
  Serial.println(wheelR.getSpeed());
#endif
}
