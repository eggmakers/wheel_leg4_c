#pragma once

#include <Arduino.h>

class LQR
{
private:
    double K[12];
    double k = 150;
    double linear_vel = 0;
    double turn_vel = 0;
    double pitch = 0;

public:
    LQR()
    {
        /*          static const double dv[12]{
                    0,      0,
                    -0.0369,-0.0369,
                    -0.0878,-0.0878,
                    -0.0060,-0.0060,
                    0,0,
                    0.0012,   -0.0012};
                static const double dv1[12]{
                    0,      0,
                    -0.0503,-0.0503,
                    -0.1714,-0.1714,
                    -0.0157,-0.0157,
                    0,0,
                    0.0016,   -0.0016};
                std::copy(&dv[0], &dv[12], &K[0]); */
    }

    // Arguments    : const double &des[6]
    //                const double &states[6]
    //                double &out[2]
    // Return Type  : void
    void setK(int index)
    {
        if (index == 0)
        {
            static const double dv[12]{
                0, 0,
                -0.0503, -0.0503,
                -0.1559, -0.1559,
                -0.0128, -0.0128,
                0, 0,
                0.0016, -0.0016};
            std::copy(&dv[0], &dv[12], &K[0]);
        }
        else if (index == 1)
        {
            static const double dv[12]{
                0, 0,
                -0.0503, -0.0503,
                -0.1639, -0.1639,
                -0.0142, -0.0142,
                0, 0,
                0.0016, -0.0016};
            std::copy(&dv[0], &dv[12], &K[0]);
        }
        else if (index == 2)
        {
            static const double dv[12]{
                0, 0,
                -0.0503, -0.0503,
                -0.1714, -0.1714,
                -0.0157, -0.0157,
                0, 0,
                0.0016, -0.0016};
            std::copy(&dv[0], &dv[12], &K[0]);
        }
        Serial.print("setK index: ");
        Serial.println(index);
    }
    void Control(const double des[6], const double states[6], double out[2])
    {
        double diff[6];
        for (int i{0}; i < 6; i++)
        {
            diff[i] = des[i] - states[i];
        }
        for (int i{0}; i < 2; i++)
        {
            double d;
            d = 0.0;
            for (int j{0}; j < 6; j++)
            {
                d += K[i + (j << 1)] * diff[j];
            }
            out[i] = d * k;
        }
    }
    void Control(const double states_[4], double out[2])
    {
        const double des[6] = {0, linear_vel, pitch, 0, 0, turn_vel};
        const double states[6] = {0, states_[0], states_[1], states_[2], 0, states_[3]};
        Control(des, states, out);
    }
    void setVel(double linear, double turn)
    {
        linear_vel = linear;
        turn_vel = turn;
    }
    void setLinearVel(double vel)
    {
        linear_vel = vel;
    }
    void setTurnVel(double vel)
    {
        turn_vel = vel;
    }
    void setPitch(double angle)
    {
        pitch = angle / 180 * M_PI;
    }

    double getLinearVel()
    {
        return linear_vel;
    }
    double getTurnVel()
    {
        return turn_vel;
    }
    double getPitch()
    {
        return pitch / M_PI * 180;
    }
};
