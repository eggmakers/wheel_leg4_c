
#pragma once

#include <Arduino.h>
#include <stdint.h>

#define MIN_PULSE_WIDTH 500      // the shortest pulse sent to a servo  us
#define MAX_PULSE_WIDTH 2500     // the longest pulse sent to a servo   us
#define DEFAULT_PULSE_WIDTH 1500 // default pulse width when servo is attached  us
#define REFRESH_INTERVAL 20000   // minimum time to refresh servos in microseconds

#define SERVO_PWM_RESOLUTION_BIT 16 // pwm分辨玿 stm32最夿16房32使
#define SERVO_PWM_RESOLUTION (pow(2, SERVO_PWM_RESOLUTION_BIT) - 1)
#define ANGLE_RESOLUTION SERVO_PWM_RESOLUTION / 10 //   (2.5ms-0.5ms)/20ms=10

class Servo
{
private:
    uint32_t pin; // pwm pin
    float angle;  // 当前角度  单位°
    float goal;
    uint32_t plus; // 当前脉宽时间 单位us
    uint64_t last;
    float speed;

protected:
    float min; // minimum angle  单位°
    float max; // maximum angle  单位°

public:
    friend class Leg;
    Servo();
    Servo(uint32_t pin, float min = 0, float max = 180) : pin(pin), min(min), max(max) {}
    static void begin()
    {
        constexpr uint32_t freq = 1e6 / REFRESH_INTERVAL;
        analogWriteFrequency(freq);                      // 频率设置 舵机pwm周期20ms，频率50hz
        analogWriteResolution(SERVO_PWM_RESOLUTION_BIT); // 设置占空比调节范围
    }
    void attach() // attach the given pin to the next free channel, sets pinMode, returns channel number or INVALID_SERVO if failure
    {
        angle = (min + max) / 2;
        goal = angle;
        speed = 0;
        pinMode(pin, OUTPUT);
    }
    void attach(float angle) // attach the given pin to the next free channel, sets pinMode, returns channel number or INVALID_SERVO if failure
    {
        attach();
        write(angle);
        goal = angle;
    }
    // uint8_t attach(int pin, int min, int max); // as above but also sets min and max values for writes.
    // void detach();
    void write(float angle) // if value is < 200 its treated as an angle, otherwise as pulse width in microseconds
    {
        int plus = map(angle, min, max, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
        writeMicroseconds(plus);
        this->angle = angle;
        // Serial.print("angle: ");Serial.println(angle);
    }
    void write(float angle, float speed) // 速度单位°/s 速度小于30以下有顿挫感
    {
        goal = angle;
        this->speed = goal > this->angle ? abs(speed) : -abs(speed);
        last = micros();
        // Serial.print("this angle:");Serial.print(this->angle);Serial.print(", goal:");Serial.print(goal);
        // Serial.print(", this->speed:");Serial.print(this->speed);Serial.print(", last:");Serial.println(last);
    }
    void writeMicroseconds(uint32_t plus) // Write pulse width in microseconds
    {
        int ulValue = plus * SERVO_PWM_RESOLUTION / REFRESH_INTERVAL;
        analogWrite(pin, ulValue);
        this->plus = plus;
    }
    float read() { return angle; }          // returns current pulse width as an angle between 0 and 180 degrees
    int readMicroseconds() { return plus; } // returns current pulse width in microseconds for this servo (was read_us() in first release)
    // bool attached();                   // return true if this servo is attached, otherwise false
    void update()
    {
        uint32_t now = micros();
        // Serial.print("speed:");Serial.print(speed);
        if (speed > 0.001)
        {
            // Serial.println("1");
            if (goal > angle + 0.01)
            {
                // Serial.println("2");
                angle = angle + (now - last) * speed / (1e6);
                write(angle);
            }
            else
            {
                speed = 0;
                angle = goal;
            }
        }
        else if (speed < -0.001)
        {
            // Serial.println("3");
            if (goal < angle - 0.01)
            {
                // Serial.println("4");
                angle = angle + (now - last) * speed / (1e6);
                write(angle);
            }
            else
            {
                speed = 0;
                angle = goal;
            }
        }
        // Serial.print("(now-last)/(1e6):");Serial.println((now-last)/(1e6));
        last = now;
    }
};