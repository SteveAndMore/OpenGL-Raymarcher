#include "maths.hpp"

std::vector<float> Resolvequadra(float a, float b, float c)
{
    std::vector<float> ret;
    float discriminant = b*b - 4*a*c;
    if (discriminant > 0) {
        float x1 = (-b + sqrt(discriminant)) / (2*a);
        float x2 = (-b - sqrt(discriminant)) / (2*a);
        ret.push_back(x1);
        ret.push_back(x2);
    }
    else if (discriminant == 0)
    {
        float x = -b/(2*a);
        ret.push_back(x);
    }
    return ret;
}