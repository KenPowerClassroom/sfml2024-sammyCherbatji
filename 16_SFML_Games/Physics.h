#pragma once
#include <cmath>

class Physics
{
public:
    static void applyThrust(float& dx, float& dy, float angleDeg, float thrustPower)
    {
        float rad = angleDeg * 0.017453f;
        dx += std::cos(rad) * thrustPower;
        dy += std::sin(rad) * thrustPower;
    }

    static void applyDrag(float& dx, float& dy, float drag)
    {
        dx *= drag;
        dy *= drag;
    }

    static void limitSpeed(float& dx, float& dy, float maxSpeed)
    {
        float speed = std::sqrt(dx * dx + dy * dy);
        if (speed > maxSpeed)
        {
            float scale = maxSpeed / speed;
            dx *= scale;
            dy *= scale;
        }
    }
};

