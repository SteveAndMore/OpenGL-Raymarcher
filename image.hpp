#pragma once
#include <fstream>
#include <vector>
#include <iostream>

class ColorRGB
{
public:
    u_int8_t r;
    u_int8_t g;
    u_int8_t b;
    ColorRGB(u_int8_t r, u_int8_t g, u_int8_t b):r(r), g(g), b(b){}
    ColorRGB():r(255), g(255), b(255){}

    ColorRGB operator*(const float f)const;
    ColorRGB operator*(const ColorRGB c)const;
};

class Pixel
{
public:
    ColorRGB color;

    Pixel(ColorRGB color):color(color){}

    Pixel(u_int8_t r, u_int8_t g, u_int8_t b):color(ColorRGB(r,g,b)){}
};

class Image
{
public:
    int h;
    int w;
    std::vector<Pixel> pixels;

    Image(int w, int h):w(w), h(h)
    {
        pixels = std::vector<Pixel>(w * h, Pixel(0,0,0));
    }
    
    void ToPPM(std::string filename);
    Pixel& At(int x, int y);//x (>) axis, y (^) axis
};