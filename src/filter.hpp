#ifndef __FILTERS_HPP
#define __FILTERS_HPP

#include <Arduino.h>
#include <stdint.h>

template <class T>
class Filter
{
private:
    T *pdata;
    uint32_t num;
    uint32_t len;
    T sum;
    uint32_t index;

public:
    static T Avg(T *data, uint32_t len)
    {
        T sum = 0;
        for (int i = 0; i < len; i++)
            sum += data[i];
        return sum / len;
    }

    Filter(uint32_t len) : len(len)
    {
        num = 0;
        index = 0;
        sum = 0;
        pdata = new T[len];
    }
    ~Filter()
    {
        if (pdata != NULL)
        {
            delete[] pdata;
            pdata = NULL;
        }
    }
    T update(T value)
    {
        if (num < len)
        {
            pdata[num] = value;
            num++;
            sum += value;
            return sum / num;
        }
        else
        {
            if (index >= len)
                index = 0;
            sum = sum - pdata[index] + value;
            pdata[index] = value;
            index++;
            return sum / len;
        }
    }
    void printf()
    {
        Serial.print(sum);
        Serial.print(" ");
        Serial.print(len);
        Serial.print(" ");
        for (int i = 0; i < len; i++)
        {
            Serial.print(pdata[i]);
            Serial.print(" ");
        }
        Serial.println(" ");
    }
};

#endif /* __FILTERS_H */