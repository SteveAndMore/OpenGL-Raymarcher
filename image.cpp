#include "image.hpp"

void Image::ToPPM(std::string filename)
{
    std::ofstream PPMfile;
    PPMfile.open (filename);
    PPMfile.clear();
    PPMfile << "P6 " << w << " " << h << " 255\n";
    for (Pixel &pixel : pixels)
    {
        PPMfile << pixel.color.r << pixel.color.g << pixel.color.b;
    }
    PPMfile.close();
}

Pixel& Image::At(int x, int y)
{
    if (x > w || y > h)
    {
        std::cout << "Image At " << x << ", " << y << " out of bounds\n";
        exit(1);
    }
    return pixels.at(x + w * y);
}

u_int8_t clamp_uint8(int i)
{
    if (i > INT8_MAX)
        i = INT8_MAX;
    if (i < INT8_MIN)
        i = INT8_MIN;
    return i;
}

ColorRGB ColorRGB::operator*(const float f)const
{
    return ColorRGB(clamp_uint8(r * f), clamp_uint8(g * f), clamp_uint8(b * f));
}

ColorRGB ColorRGB::operator*(const ColorRGB c)const
{
    return ColorRGB(clamp_uint8((float)c.r/255 * r), clamp_uint8((float)c.g/255 * g), clamp_uint8((float)c.b/255 * b));
}