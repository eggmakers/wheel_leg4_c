// #define SERIAL_UART_INSTANCE 1 //�޸�Ĭ�ϴ���1��ϵͳĬ�ϴ���4����Ϊmpu6050��ʹ��Serial�����Խ��䶨�嵽��ͷ�ļ�د
// stm32 ������Ҫ��Ӣ��Ŀ¼�£�������뱨��
// �궨����Զ�����c_cpp_properties.json�ļ��У�����ʱ������
// ���������ߵ��ԣ����Զ�����jflash���
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