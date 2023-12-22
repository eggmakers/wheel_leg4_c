#pragma once

#include <Arduino.h>
#include <map>

#define DIVIDER1 3300  // 分压电阻1  单位欧姆
#define DIVIDER2 10000 // 分压电阻2  单位欧姆
#define BAT_NUM 3      // 电池节数
#define FILTER_LEN 12
class Power
{
private:
    uint32_t pin;
    int power; // 电量百分比 -1表示还未初始化，

public:
    static std::map<float, int> table18650;
    Power(uint32_t pin) : pin(pin) {}
    ~Power() {}
    void bigen()
    {
        pinMode(pin, INPUT);
        float avg = 0;
        for (int i = 0; i < 50; i++)
        {
            avg += getOneVoltage() / 50;
            delay(5);
        }
        power = VoltageToQuantity(avg);
    }
    void bigen(uint32_t pin)
    {
        this->pin = pin;
        bigen();
    }
    uint32_t getAdcDigit()
    {
        return analogRead(pin);
    }
    float getAdcAnalog()
    {
        constexpr float adcK = 3.3 / 1023;
        return getAdcDigit() * adcK;
    }
    float getPowerVoltage()
    {
        constexpr float volK = ((float)DIVIDER1 + DIVIDER2) / DIVIDER1;
        return getAdcAnalog() * volK;
    }
    float getOneVoltage()
    {
        return getPowerVoltage() / BAT_NUM;
    }
    int VoltageToQuantity(float vol)
    {
        for (auto p : table18650)
        {
            if (vol < p.first)
                return p.second;
        }
        auto p = --table18650.end();
        return p->second;
    }
    int getQuantity()
    {
        float vol = getOneVoltage();
        return VoltageToQuantity(vol);
    }

    /*
    A、名称：消抖滤波泿
    B、方法：
        设置一个滤波计数器，将每次采样值与当前有效值比较：
        如果采样倿=当前有效值，则计数器清零＿
        如果采样倿<>当前有效值，则计数器+1，并判断计数器是吿>=上限N（溢出）＿
        如果计数器溢出，则将本次值替换当前有效值，并清计数器?
    C、优点：
        对于变化缓慢的被测参数有较好的滤波效果；
        可避免在临界值附近控制器的反复开/关跳动或显示器上数值抖动?
    D、缺点：
        对于快速变化的参数不宜＿
        如果在计数器溢出的那一次采样到的值恰好是干扰倿,则会将干扰值当作有效值导入系统?
    E、整理：shenhaiyu 2013-11-01
    */
    int getFilterQuantity()
    {
        int count = 0;
        int new_value = getQuantity(); /*读取采样倿*/
        while (power != new_value)
        {
            count++;
            if (count >= FILTER_LEN)
            {
                power = new_value;

                return new_value;
            }
            new_value = getQuantity();
        }
        return power;
    }
};
