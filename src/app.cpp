#include "hal.h"
#include "leg.hpp"
#include "store.hpp"

#include "STM32TimerInterrupt.h"
#include <PID_v2.h>

#include <STM32FreeRTOS.h>
#include "LQR.hpp"

#define LEG_MAX 35
#define LEG_MIN 17
#define LEG_LENGTH (LEG_MAX - LEG_MIN + 1)

Leg legL(srvL, false, 40, 15);
Leg legR(srvR, true, 40, 15);
float zero[LEG_LENGTH] = {0.0f, 0.0f, 0.0f};
int legIndex;
float nowZero = 0;
LQR lqr;
float avr = (LEG_MAX + LEG_MIN) / 2; // Average leg length
float diff = 0;
Store store(eep);

void chooseZeroK()
{
  legIndex = round((legL.get() + legR.get()) / 2) - LEG_MIN; // The min angle of leg is 17(maybe need change)
  lqr.setPitch(zero[legIndex]);                              // set zero bias
  Serial.print("legIndex: ");
  Serial.println(legIndex);
  Serial.print("zero[legIndex]: ");
  Serial.println(zero[legIndex]);
  if (legIndex < (LEG_MAX - LEG_MIN) / 3) // after leg angle is less than 6
  {
    lqr.setK(0);
  }
  else if (legIndex < (LEG_MAX - LEG_MIN) * 2 / 3) // after leg angle is less than 12
  {
    lqr.setK(1);
  }
  else
  {
    lqr.setK(2);
  }
}

void leg_init()
{
  float angle;
  // store.put(1,angle);
  store.get(1, angle);
  uint8_t *p = (uint8_t *)(&angle);
  if (p[0] == 0xff && p[1] == 0xff && p[2] == 0xff && p[3] == 0xff)
  {
    Serial.println("leg l angle数据未初始化,启用默认值90");
    angle = 90;
  }
  Serial.print("angle l: ");
  Serial.println(angle);
  legL.begin(angle);
  // legL.set(30,30);

  // store.put(2,angle);
  store.get(5, angle);
  if (p[0] == 0xff && p[1] == 0xff && p[2] == 0xff && p[3] == 0xff)
  {
    Serial.println("leg r angle数据未初始化,启用默认值90");
    angle = 90;
  }
  Serial.print("angle r: ");
  Serial.println(angle);
  legR.begin(angle);

  // avr=(legR.get()+legL.get())/2;
  // legR.set(30,30);
  chooseZeroK();
}

void leg_update()
{
  float angle;
  if (legL.isChenge())
  {
    angle = legL.get();
    store.put(1, angle); // id is 1
    Serial.print("angle_l: ");
    Serial.println(angle);
    chooseZeroK();
  }
  if (legR.isChenge())
  {
    angle = legR.get();
    store.put(5, angle); // id is 5
    Serial.print("angl_R: ");
    Serial.println(angle);
    chooseZeroK();
  }
}

static long time2;
STM32Timer ITimer(TIM4);

// 有的时候会自己转圈，可能和轮子测速的数据不对有关
// 可以调节K值 ，转向速度， 行驶速度

double wr = 0.036 / 2;
double d = 0.106; // 轮距
// double distance_k=3;
void TimerHandler()
{
  double states[4] = {(wheelL.getSpeed() + wheelR.getSpeed()) * wr / 2, -mpu6050.getAngleY() / 180 * M_PI, -mpu6050.getGyroY() / 180 * M_PI, (wheelL.getSpeed() - wheelR.getSpeed()) * wr / d}; //-mpu6050.getAngleY()/180*M_PI
  // double states[4]={0,0,0,(wheelL.getSpeed()-wheelR.getSpeed())*wr/d};//-mpu6050.getAngleY()/180*M_PI

  double out[2];
  lqr.Control(states, out);
  // stand_out = stand.Run(mpu6050.getAngleY(),mpu6050.getGyroY()/100.0);
  // Serial.print(" ls: ");Serial.print(wheelL.getSpeed());Serial.print(" rs: ");Serial.print(wheelR.getSpeed());Serial.print(" st: ");Serial.print(stand_out);Serial.print(" sp: ");Serial.print(speed_out);Serial.print(" t: ");Serial.println(turn_out);
  /*   Serial.print("AngleY(): ");Serial.print(mpu6050.getAngleY());
    Serial.print(" ls: ");Serial.print(wheelL.getSpeed());Serial.print(" rs: ");Serial.print(wheelR.getSpeed());
    Serial.print(" speed: ");Serial.print(states[0]);
    Serial.print("out: ");Serial.print(out[0]);Serial.println(out[1]); */
  wheelL.setTorque(out[0]);
  wheelR.setTorque(out[1]);
  Serial.println("-");
}

void get_par()
{
  float data[12] = {0};
  store.get(11, data); // 11是地址，不可乱动
  Serial.println("get par: ");
  for (int i = 0; i < 12; i++)
  {
    uint8_t *p = (uint8_t *)(data + i);
    if (p[0] == 0xff && p[1] == 0xff && p[2] == 0xff && p[3] == 0xff)
    {
      Serial.print(i);
      Serial.println(": 数据未初始化,启用默认值0");
      data[i] = 0;
    }
    // Serial.print(p[0]);Serial.print(": ");Serial.print(p[1]);Serial.print(": ");Serial.print(p[2]);Serial.print(": ");Serial.print(p[3]);Serial.println(": ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(data[i]);
  }

  // lqr.setPitch(data[9]);
  Motor::setDeadZone(data[10]);
}

void get_zero()
{
  store.get(60, zero); // 11是地址，不可乱动
  Serial.println("get zero: ");
  for (int i = 0; i < LEG_LENGTH; i++)
  {
    uint8_t *p = (uint8_t *)(zero + i);
    if (p[0] == 0xff && p[1] == 0xff && p[2] == 0xff && p[3] == 0xff)
    {
      Serial.print(i);
      Serial.println(": zero数据未初始化,启用默认值0");
      zero[i] = 0;
    }
    // Serial.print(p[0]);Serial.print(": ");Serial.print(p[1]);Serial.print(": ");Serial.print(p[2]);Serial.print(": ");Serial.print(p[3]);Serial.println(": ");
    Serial.print("leg angle: ");
    Serial.print(i + 15);
    Serial.print(": ");
    Serial.println(zero[i]);
  }
}
void set_zero()
{
  Serial.print("set zero: ");
  for (int i = 0; i < LEG_LENGTH; i++)
  {
    Serial.print("leg angle: ");
    Serial.print(i + 15);
    Serial.print(": ");
    Serial.println(zero[i]);
  }
  store.put(60, zero); // 11是地址，不可乱动
}

void set_par()
{
  float data[12] = {0};
  // data[9]=lqr.getPitch();
  data[10] = Motor::getDeadZone();
  store.put(11, data); // 11是地址，不可乱动
  Serial.print("set par: ");
  for (int i = 0; i < 12; i++)
  {
    Serial.print(i);
    Serial.print(": ");
    Serial.println(data[i]);
  }
}

Wireless wire(Serial2);
void wifi_write()
{
  wire.write(wheelL.getSpeed(),
             wheelR.getSpeed(),
             mpu6050.getAngleY(),
             power.getFilterQuantity(),
             100, time2, 300, 400, 500);
  // wire.serial.printf("(%f , %f , %f , %f\n",wheelL.getSpeed(),wheelR.getSpeed(),mpu6050.getAngleY(),power.getFilterQuantity());
}
void wifi2_write()
{
  float data[8] = {0};
  data[0] = wheelL.getSpeed();
  data[1] = wheelR.getSpeed();
  data[2] = mpu6050.getAngleY();
  data[3] = mpu6050.getGyroY();
  data[7] = 1;
  wire.write2(data);
}
void wifi_update()
{
  __IntervalExecute(wifi_write(), 100);
  __IntervalExecute(wifi2_write(), 100);
}

Button button(PC8, B_PULLUP, 1000);
void button_update()
{
  short result = button.checkPress();
  /* Examining the previous result and writing if the pressure is short or long to Serial Monitor. */
  if (result == 1)
  {
    Serial.println("SHORT PRESS!");
    ITimer.enableTimer();
  }
  else if (result == -1)
  {
    Serial.println("LONG PRESS!");
    ITimer.disableTimer();
  }
}

double linear_vel = 0.2; // Distance of forward,the default is 0.2
double turn_vel = 4;     // diriction of turn,the default is 4

void cmd_handle()
{
  String str = wire.read();
  if (str == "Square") // □
  {
    Serial.println("Square");
  }
  else if (str == "Triangle") // △
  {
    Serial.println("Triangle");
  }
  else if (str == "Cross") //  X
  {
    ITimer.disableTimer();
    Serial.println("Cross");
    wheelL.setTorque(0);
    wheelR.setTorque(0);
  }
  else if (str == "Circle") // O
  {
    wheelL.setTorque(0);
    wheelR.setTorque(0);
    ITimer.enableTimer();
    Serial.println("Circle");
  }
  else if (str.charAt(0) == 'R' && str.charAt(6) == 'X')
  {
    float value = str.substring(8).toFloat();
    float temp;
    if (value > 20)
      temp = __Map(value, 20, 128, 0, turn_vel);
    else if (value < -20)
      temp = __Map(value, -128, -20, -turn_vel, 0);
    else
      temp = 0;
    lqr.setTurnVel(temp);
    Serial.print("R X:");
    Serial.println(temp);
  }
  else if (str.charAt(0) == 'R' && str.charAt(6) == 'Y')
  {
    float value = str.substring(8).toFloat();
    float temp;
    if (value > 20)
      temp = __Map(value, 20, 128, 0, linear_vel);
    else if (value < -20)
      temp = __Map(value, -128, -20, -linear_vel, 0);
    else
      temp = 0;
    lqr.setLinearVel(temp);
    Serial.print("R Y:");
    Serial.println(temp);
  }
  else if (str.charAt(0) == 'L' && str.charAt(6) == 'X')
  {
    float value = str.substring(8).toFloat();

    float temp;
    if (value > 20)
      temp = __Map(value, 20, 128, 0, LEG_LENGTH / 3);
    else if (value < -20)
      temp = __Map(value, -128, -20, -LEG_LENGTH / 3, 0);
    else
      temp = 0;
    diff = temp;
    legR.set(avr - diff, 20);
    legL.set(avr + diff, 20);
    Serial.print("L X:");
    Serial.println(temp);
  }
  else if (str.charAt(0) == 'L' && str.charAt(6) == 'Y')
  {
    float value = str.substring(8).toFloat();
    /* if(value>120)
    {
      legR.set(LEG_MAX,20);
      legL.set(LEG_MAX,20);
    }
    else if(value<-120)
    {
      legR.set(LEG_MIN,20);
      legL.set(LEG_MIN,20);
    }
    else
    {
      legR.set(legR.get(),0);
      legL.set(legL.get(),0);
    } */
    float temp;
    if (value > 20)
      temp = __Map(value, 20, 128, (LEG_MAX + LEG_MIN) / 2, LEG_MAX);
    else if (value < -20)
      temp = __Map(value, -128, -20, LEG_MIN, (LEG_MAX + LEG_MIN) / 2);
    else
      temp = (LEG_MAX + LEG_MIN) / 2;
    avr = temp;
    legR.set(avr - diff, 20);
    legL.set(avr + diff, 20);
    Serial.print("L Y:");
    Serial.println(temp);
  }

  else if (str.charAt(0) == 'C')
  { // 前进
    lqr.setVel(linear_vel, 0);
  }
  else if (str.charAt(0) == 'A')
  { // 左转
    lqr.setVel(0, -turn_vel);
  }
  else if (str.charAt(0) == 'G')
  { ////后退
    lqr.setVel(-linear_vel, 0);
  }
  else if (str.charAt(0) == 'E')
  { // 右转
    lqr.setVel(0, turn_vel);
  }
  else if (str.charAt(0) == 'Z')
  { // 停
    lqr.setVel(0, 0);
  }
  else if (str == "a") // 调零位
  {
    lqr.setPitch(lqr.getPitch() + 0.1);
    zero[legIndex] = lqr.getPitch();
    Serial.print("zero[legIndex]: ");
    Serial.println(zero[legIndex]);
  }
  else if (str == "b")
  {
    lqr.setPitch(lqr.getPitch() - 0.1);
    zero[legIndex] = lqr.getPitch();
    Serial.print("zero[legIndex]: ");
    Serial.println(zero[legIndex]);
  }
  else if (str == "c") // 调死区
  {
    float dead = Motor::getDeadZone() + 0.01;
    Motor::setDeadZone(dead);
  }
  else if (str == "d")
  {
    float dead = Motor::getDeadZone() - 0.01;
    Motor::setDeadZone(dead);
  }
  else if (str == "e")
  {
    float angle = legL.get() + 1;
    legL.set(angle, 20);
    angle = legR.get() + 1;
    legR.set(angle, 20);
  }
  else if (str == "f")
  {
    float angle = legL.get() - 1;
    legL.set(angle, 20);
    angle = legR.get() - 1;
    legR.set(angle, 20);
  }
  else if (str == "g")
  {
    float angle = legL.get() + 1;
    legL.set(angle, 20);
  }
  else if (str == "h")
  {
    float angle = legL.get() - 1;
    legL.set(angle, 20);
  }
  else if (str == "i") // 调零位
  {
    for (int i = 0; i < LEG_LENGTH; i++)
    {
      zero[i] += 0.1;
      Serial.print("zero[i]: ");
      Serial.println(zero[i]);
    }
    lqr.setPitch(lqr.getPitch() + 0.1);
    zero[legIndex] = lqr.getPitch();
    Serial.print("zero[legIndex]: ");
    Serial.println(zero[legIndex]);
  }
  else if (str == "j")
  {
    for (int i = 0; i < LEG_LENGTH; i++)
    {
      zero[i] -= 0.1;
      Serial.print("zero[i]: ");
      Serial.println(zero[i]);
    }
    lqr.setPitch(lqr.getPitch() - 0.1);
    zero[legIndex] = lqr.getPitch();
    Serial.print("zero[legIndex]: ");
    Serial.println(zero[legIndex]);
  }
  else if (str == String("Q:P"))
  {
  }
  else if (str == String("Q:W"))
  {
    set_par();
    set_zero();
  }
  else if (str.charAt(0) == '0')
  {
    String temp = str;
    temp.remove(0, 2);
    linear_vel = temp.toFloat() / 1000.0;
  }
  else if (str.charAt(0) == '1')
  {
    String temp = str;
    temp.remove(0, 2);
    turn_vel = temp.toFloat() / 1000.0;
  }
  else if (str.charAt(0) == '2')
  {
    String temp = str;
    temp.remove(0, 2);
  }
  else if (str.charAt(0) == '3')
  {
    String temp = str;
    temp.remove(0, 2);
  }
  else if (str.charAt(0) == '4')
  {
    String temp = str;
    temp.remove(0, 2);
  }
  else if (str.charAt(0) == '5')
  {
    String temp = str;
    temp.remove(0, 2);
  }
  else if (str.charAt(0) == '6')
  {
    String temp = str;
    temp.remove(0, 2);
  }
  else if (str.charAt(0) == '7')
  {
    String temp = str;
    temp.remove(0, 2);
  }
  else if (str.charAt(0) == '8')
  {
    String temp = str;
    temp.remove(0, 2);
  }
  else if (str.charAt(0) == '#')
  {
    Serial.println("111");
  }
  else
  {
    if (str.length() >= 1)
    {
      Serial.print("cmd str: ");
      Serial.println(str);
    }
  }
}

void TaskDisplay(void *pvParameters __attribute__((unused)))
{
  for (;;) // A Task shall never return or exit.
  {
    u8g2.clearBuffer(); // 清屏
    // u8g2.drawStr(0, 16, "zero: "); //注意这里是0,0开始显示Hello World
    u8g2.setCursor(0, 7); // 设置打印起始地址（下面的t显示在什么地方）
    u8g2.print("zero:");
    u8g2.print(lqr.getPitch(), 2);
    u8g2.print(" dead:");
    u8g2.print(Motor::getDeadZone(), 2);
    u8g2.setCursor(0, 15);
    u8g2.print("legR:");
    u8g2.print(legR.get(), 1);
    u8g2.print(" legL:");
    u8g2.print(legL.get(), 1);
    u8g2.setCursor(0, 23);
    u8g2.print("linearV:");
    u8g2.print(linear_vel, 2);
    u8g2.print(" turnV:");
    u8g2.print(turn_vel, 2);
    u8g2.setCursor(0, 31);
    u8g2.print("quant:");
    u8g2.print(power.getFilterQuantity()); // 显示电量
    u8g2.sendBuffer();                     // 将以上所有的缓存内容显示
    vTaskDelay(100);
  }
}

void app_init()
{
  get_zero();
  leg_init();
  wire.begin();
  // Interval in microsecs
  get_par();

  if (!ITimer.attachInterruptInterval(10000, TimerHandler))
    Serial.println(F("Can't set ITimer. Select another freq. or timer"));
  ITimer.disableTimer();

  xTaskCreate(
      TaskDisplay, (const portCHAR *)"TaskDisplay" // A name just for humans
      ,
      512 // This stack size can be checked & adjusted by reading the Stack Highwater
      ,
      NULL, 0 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
      ,
      NULL);
  // start scheduler
  vTaskStartScheduler();
  // 此处的程序执行不到
}

void app_update()
{
  static long time = micros();
  time2 = micros() - time;
  time = micros();
  leg_update();
  button_update();
  wifi_update();
  cmd_handle();

  //__IntervalExecute(display(), 100);
}
