#pragma once

#include <Arduino.h>

// #define DEADZONE 0  //0.3
class Motor
{
private:
    /* data */
    HardwareSerial &serial;
    float speed = 0;
    float torque = 0;
    bool set = true; //
    bool get = true;
    static float deadzone;

public:
    Motor(HardwareSerial &serial) : serial(serial){};
    void begin(bool get)
    {
        // 串口1，初始化串口，并重新定义引脚
        this->get = get;
        Serial.print("motor init");
        int times = 3;
        while (times > 0)
        {
            times--;
            delay(300);
            while (serial.read() > 0)
                ;
            serial.print("@1\n"); // 设置verbose模式�? on_request
            Serial.print(".");
            String str = serial.readStringUntil('\n');
            Serial.println(str);
            str.remove(3);
            if (str == String("on!")) // readString()方法延时时间太长，readStringUntil('\n')方法延时到截至符'\n'
                break;
        }
        Serial.println("end");
        setTorque(0, false);
        while (serial.read() > 0)
            ;
    }

    void setTorque(float value, bool dead = true)
    {
        if (dead)
            torque = value + (value > 0 ? deadzone : -deadzone);
        else
            torque = value;
        set = true;
    }
    static void setDeadZone(float dead)
    {
        deadzone = dead;
    }
    static float getDeadZone()
    {
        return deadzone;
    }

    float getSpeed()
    {
        return speed;
    }

    void update()
    {
        // while(serial.read()>0);
        if (get && serial.available())
        {
            String str = serial.readStringUntil('\n');
            // Serial.print("1: "); Serial.println(str);
            speed = str.toFloat();
            // Serial.println(speed);
            str = serial.readStringUntil('\n');
            // Serial.print("2: "); Serial.println(str);
            while (serial.read() > 0)
                ;
        }
        if (set && serial.available())
        {
            // String str=serial.readStringUntil('\n');
            // Serial.print("motor:");Serial.println(str);
            // if(abs(str.toFloat()-torque)<0.1)
            //     set=false;
        }
        // get speed
        if (get)
        {
            serial.print("MMG5\n");
            /*          Serial.println(serial.readStringUntil(':'));
                        String str=serial.readStringUntil('\n');
                        speed=str.toFloat();
                        Serial.println(str);
                        Serial.println(speed); */
            // speed=-serial.readStringUntil('\n').toFloat();//2ms的时�?
        }
        // set torque
        if (set)
        {
            serial.print(String("T") + String(torque) + String("\n"));
            // delay(10);
            /* String str=serial.readStringUntil('\n');
            if(abs(str.toFloat()-torque)<0.1)
                set=false;
            Serial.println(str); */
        }
    }
};

class Wheel
{
private:
    /* data */
    bool dir;
    Motor &motor;

public:
    Wheel(Motor &motor, bool dir) : motor(motor), dir(dir){};

    void setTorque(float value, bool dead = true)
    {
        value = dir ? value : -value;
        motor.setTorque(value, dead);
    }

    float getSpeed()
    {
        float speed = motor.getSpeed();
        speed = dir ? speed : -speed;
        return speed;
    }
};
