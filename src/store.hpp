#pragma once

#include <Arduino.h>
#include <extEEPROM.h>

class Store
{
private:
    /* data */
    extEEPROM &eep;

public:
    Store(extEEPROM &eep) : eep(eep) {}
    template <typename T>
    T &get(const int &idx, T &t)
    {
        uint16_t offset = idx;
        uint8_t *_pointer = (uint8_t *)&t;
        eep.read(offset, _pointer, sizeof(T));
        return t;
    }
    template <typename T>
    const T &put(const int &idx, const T &t)
    {
        // Copy the data from the flash to the buffer if not yet
        uint16_t offset = idx;
        uint8_t *_pointer = (uint8_t *)&t;
        if (sizeof(T) == 1)
            eep.update(offset, *_pointer);
        else
            eep.write(offset, _pointer, sizeof(T));

        return t;
    }
};

class Wireless
{
private:
    /* data */

    // int txbuf[9]={1,2,3,4,5,6,7,8,9};// speedL speedR power angle  data1 data2 data3 data4 data5
    String rxstr;

public:
    HardwareSerial &serial;
    Wireless(HardwareSerial &serial) : serial(serial){};
    void begin()
    {
        while (serial.read() > 0)
            ;
    }
    /*     void setTxbuf(int speedl,int speedr,int power,int angle,int data1,int data2,int data3,int data4,int data5)
        {
            txbuf[0]=speedl;
            txbuf[1]=speedr;
            txbuf[2]=angle;
            txbuf[3]=power;
            txbuf[4]=data1;
            txbuf[5]=data2;
            txbuf[6]=data3;
            txbuf[7]=data4;
            txbuf[8]=data5;

        }
        void write()
        {
            serial.print("{A");serial.print(txbuf[0]);   //×óÂÖ±àÂë‡’
            serial.print(":"); serial.print(txbuf[1]);    //ÓÒÂÖ±àÂë‡’
            serial.print(":"); serial.print(txbuf[2]);  //µç³ØµçÑ¹
            serial.print(":"); serial.print(txbuf[3]);    //Æ½ºâÇã½Ç
            serial.print("}$");
            serial.print("{B");serial.print(txbuf[4]);
            serial.print(":");serial.print(txbuf[5]);
            serial.print(":");serial.print(txbuf[6]);
            serial.print(":");serial.print(txbuf[7]);
            serial.print(":");serial.print(txbuf[8]);
            serial.print("}$");
        } */
    void write(int speedl, int speedr, int power, int angle, int data1, int data2, int data3, int data4, int data5)
    {
        serial.print("{A");
        serial.print(speedl); // ×óÂÖ±àÂë‡’
        serial.print(":");
        serial.print(speedr); // ÓÒÂÖ±àÂë‡’
        serial.print(":");
        serial.print(angle); // µç³ØµçÑ¹
        serial.print(":");
        serial.print(power); // Æ½ºâÇã½Ç
        serial.print("}$");
        serial.print("{B");
        serial.print(data1);
        serial.print(":");
        serial.print(data2);
        serial.print(":");
        serial.print(data3);
        serial.print(":");
        serial.print(data4);
        serial.print(":");
        serial.print(data5);
        serial.print("}$");
    }
    void write2(float (&data)[8])
    {
        serial.print("("); //%d , %d , %d , %d\n
        for (int i = 0; i < 7; i++)
        {
            serial.print(data[i]);
            serial.print(", ");
        }
        serial.printf("\n");
    }

    String &read()
    {
        rxstr = "";
        if (serial.available()) // Èç¹ûÓĞÊÕµ½Êı’İ
        {
            char cmd = serial.read();
            if ((cmd >= 'a' && cmd <= 'z') || (cmd >= 'A' && cmd <= 'Z'))
            {
                rxstr = String(cmd);
                // Serial.print(" cmd: ");Serial.println(rxstr);
                return rxstr;
            }
            else if (cmd == '{')
            {
                rxstr = serial.readStringUntil('}');
                Serial.print(" cmd: ");
                Serial.println(rxstr);
                return rxstr;
            }
            else if (cmd == '<')
            {
                rxstr = serial.readStringUntil('>');
                // Serial.print(serial.available());Serial.print(" cmd: ");Serial.println(rxstr);
                return rxstr;
            }
            else
            {
                while (serial.read() > 0)
                    ;
            }
        }
        return rxstr;
    }

    void writePid(int sp, int si, int sd, int pp, int pi, int pd, int tp, int ti, int td)
    {
        serial.print("{C");
        serial.print(sp);
        serial.print(":");
        serial.print(si);
        serial.print(":");
        serial.print(sd);
        serial.print(":");
        serial.print(pp);
        serial.print(":");
        serial.print(pi);
        serial.print(":");
        serial.print(pd);
        serial.print(":");
        serial.print(tp);
        serial.print(":");
        serial.print(ti);
        serial.print(":");
        serial.print(td);
        serial.print("}$");
    }
};
