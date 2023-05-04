#pragma once
#include "geometri.hpp"
#include "image.hpp"

#define IMG_H 500
#define IMG_W 500

class Light
{
public:
    ColorRGB color;
    Point3 pos;
    float I;
};

class Point_Light : public Light
{
public:
    Point_Light(Point3 pos, ColorRGB color, float I){this->color = color; this->pos = pos; this->I = I;}
    Point_Light(Point3 pos, float I){this->pos = pos; this->I = I;}
    Point_Light(float x, float y, float z, ColorRGB color, float I){this->color = color; this->pos = Point3(x,y,z); this->I = I;}
    Point_Light(float x, float y, float z, float I){this->pos = Point3(x,y,z); this->I = I;}
};

class Cammera
{
public:
    int Pixel_h = IMG_H;
    int Pixel_w = IMG_W;
    Point3 center;
    Point3 target;
    Vector3 up;
    float a, b, zmin;
    Cammera (Point3 center, Point3 target, Vector3 up,float a, float b, float zmin)
    :center(center), target(target), up(up), a(a), b(b), zmin(zmin)
    {
        h = tanf(b * radian_convert) * zmin * 2;
        w = tanf(a * radian_convert) * zmin * 2;
        Ptr_ImageCenter = std::make_unique<Point3>(center + (Vector3(center, target).normalize()) * zmin);
    }
    std::unique_ptr<Point3> Ptr_ImageCenter;
    float h;
    float w;

    std::vector<Point3> GetPixelPos();
};