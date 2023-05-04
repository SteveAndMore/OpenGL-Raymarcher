#pragma once
#include <iostream>
#include <math.h>
#define PI 3.14159265

static float const radian_convert  = PI / 180.0;

class Point3
{
public:
    float x;
    float y;
    float z;
    Point3():x(0), y(0), z(0){}
    Point3(float x, float y, float z):x(x), y(y), z(z){}
    Point3(Point3 const &p):x(p.x), y(p.y), z(p.z){}
    Point3(std::unique_ptr<Point3> const p):x(p->x), y(p->y), z(p->z){}
};
std::ostream& operator<<(std::ostream &out, Point3 &point);

class Vector3
{
public:
    float x;
    float y;
    float z;
    Vector3(float x, float y, float z):x(x), y(y), z(z){}
    Vector3(Point3 p1, Point3 p2):x(p2.x - p1.x), y(p2.y - p1.y), z(p2.z - p1.z){}
    
    Vector3 operator*(const float &l)const;
    Vector3 operator-(const Vector3 &v)const;
    Vector3 operator+(const Vector3 &v)const;
    Vector3 operator^(const Vector3 &v)const;
    float scalaire (const Vector3 &v)const;
    float norm();
    Vector3 normalize();
};
std::ostream& operator<<(std::ostream &out, Vector3 &vect);
Point3 operator+(const Point3 &p, const Vector3 &v);

class Line
{
public:
    Point3 point;
    Vector3 dir;
    Line(Point3 point, Vector3 dir):point(point), dir(dir){}
    Line(Point3 point, Point3 dir):point(point), dir(Vector3(point, dir)){}
};
std::ostream& operator<<(std::ostream &out, Line &line);

Vector3 Symetric(Point3 point, Line normale);