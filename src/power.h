#pragma once

#include <Arduino.h>
#include <map>

#define DIVIDER1 3300  // ��ѹ����1  ��λŷķ
#define DIVIDER2 10000 // ��ѹ����2  ��λŷķ
#define BAT_NUM 3      // ��ؽ���
#define FILTER_LEN 12
class Power
{
private:
    uint32_t pin;
    int power; // �����ٷֱ� -1��ʾ��δ��ʼ����

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
    A�����ƣ������˲���
    B��������
        ����һ���˲�����������ÿ�β���ֵ�뵱ǰ��Чֵ�Ƚϣ�
        ���������=��ǰ��Чֵ��������������
        ���������<>��ǰ��Чֵ���������+1�����жϼ������ǅ�>=����N���������
        ���������������򽫱���ֵ�滻��ǰ��Чֵ�����������?
    C���ŵ㣺
        ���ڱ仯�����ı�������нϺõ��˲�Ч����
        �ɱ������ٽ�ֵ�����������ķ�����/����������ʾ������ֵ����?
    D��ȱ�㣺
        ���ڿ��ٱ仯�Ĳ������ˣ�
        ����ڼ������������һ�β�������ֵǡ���Ǹ��ł�,��Ὣ����ֵ������Чֵ����ϵͳ?
    E������shenhaiyu 2013-11-01
    */
    int getFilterQuantity()
    {
        int count = 0;
        int new_value = getQuantity(); /*��ȡ������*/
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
