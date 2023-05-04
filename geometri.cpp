#include "geometri.hpp"

Point3 operator+(const Point3 &p, const Vector3 &v)
{
    return Point3(p.x + v.x, p.y + v.y, p.z + v.z);
}
std::ostream& operator<<(std::ostream &out, Point3 &point)
{
    out << "[" << point.x << ", " << point.y << ", " << point.z << "]";
    return out;
}

Vector3 Vector3::operator*(const float &l)const
{
    return Vector3(x * l, y * l, z * l);
}

Vector3 Vector3::operator-(const Vector3 &v)const
{
    return Vector3(x - v.x, y - v.y, z - v.z);
}

Vector3 Vector3::operator+(const Vector3 &v)const
{
    return Vector3(x + v.x, y + v.y, z + v.z);
}

Vector3 Vector3::operator^(const Vector3 &v)const
{
    return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}

std::ostream& operator<<(std::ostream &out, Vector3 &vect)
{
    out << "{" << vect.x << ", " << vect.y << ", " << vect.z << "}";
    return out;
}

std::ostream& operator<<(std::ostream &out, Line &line)
{
    out << "<" << line.point << ", " << line.dir << ">";
    return out;
}

float Vector3::scalaire(const Vector3 &v)const
{
    return v.x * x + v.y * y + v.z * z;
}

float Vector3::norm()
{
    return sqrt(x*x + y*y + z*z);
}

Vector3 Vector3::normalize()
{
    float n = norm();
    return Vector3(x/n, y/n, z/n);
}

Vector3 Symetric(Point3 point, Line line)
{
    auto v1 = Vector3(point, line.point).normalize();
    return v1 + line.dir.normalize();
}