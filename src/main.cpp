// #define SERIAL_UART_INSTANCE 1 //修改默认串口1，系统默认串叿4，因为mpu6050中使用Serial，所以将其定义到其头文件丿
// stm32 工程需要再英文目录下，否则编译报错
// 宏定义可以定义在c_cpp_properties.json文件中，但有时不好用
// 还可以在线调试，会自动调用jflash软件
#include "hal.h"
#include "app.h"

void setup()
{
  Serial.println("hello wheel leg 3");
  hal_init();

  app_init();
  Serial.println("hello wheel leg 3");
}

void loop()
{
  // long now=micros();
  hal_update();
  app_update();
  // now=micros()-now;
  // Serial.print("time ms: ");Serial.println(now/1000);
}