#pragma once

#include "servo.hpp"

class Leg
{
private:
    /* data */
    Servo &servo;
    bool dir; // true->正向   false->逆向
    float upLimit;
    float downLimit;
    float angle;

public:
    Leg(Servo &servo, bool dir, float upLimit, float downLimit) : servo(servo), dir(dir), upLimit(upLimit), downLimit(downLimit) {}
    void begin(float angle)
    {
        this->angle = angle;
        if (angle > upLimit)
            angle = upLimit;
        else if (angle < downLimit)
            angle = downLimit;
        if (dir)
            angle = angle;
        else
            angle = servo.max - (angle - servo.min);
        servo.attach(angle);
    }
    bool isChenge() // 角度是否变化
    {
        float now;
        if (dir)
            now = servo.angle;
        else
            now = servo.max - servo.angle + servo.min;
        if (abs(servo.speed) > 0.001)
        {
            if (abs(now - angle) > 1)
            {
                angle = now;
                return true;
            }
            else
                return false;
        }
        {
            if (abs(now - angle) > 0.01)
            {
                angle = now;
                return true;
            }
            else
                return false;
        }
    }
    float get()
    {
        return angle;
    }
    void set(float angle, float speed)
    {
        if (angle > upLimit)
            angle = upLimit;
        else if (angle < downLimit)
            angle = downLimit;
        if (dir)
        {
            speed = speed;
            angle = angle;
        }
        else
        {
            speed = -speed;
            angle = servo.max - (angle - servo.min);
        }
        // Serial.println(angle);
        servo.write(angle, speed);
    }
};
