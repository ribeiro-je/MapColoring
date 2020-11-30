#ifndef POSE_HH
#define POSE_HH

#include <stdio.h>

class Pose {
  public:
    float x;
    float y;
    float t;

    Pose() : x(0.0f), y(0.0f), t(0.0f) {}
    Pose(float xx, float yy, float tt)
        : x(xx), y(yy), t(tt)
    {}

    std::string
    to_s()
    {
        char temp[100];
        snprintf(temp, 100, "[%.02f,%.02f,%.02f]", x, y, t);
        return std::string(temp);
    }
};

#endif
