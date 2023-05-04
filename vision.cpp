#include "vision.hpp"

std::vector<Point3> Cammera::GetPixelPos()
{
    std::vector<Point3> ret;
    Vector3 Img_side = (up^Vector3(center, target)).normalize();
    Vector3 Img_up = up.normalize();
    float pixel_dist_h = h/Pixel_h;
    float pixel_dist_w = w/Pixel_w;
    Point3 haut_gauche = *Ptr_ImageCenter + (Img_up * (h/2) - Img_up * (pixel_dist_h/2)) + (Img_side * (w/-2) - Img_side * (pixel_dist_w/-2));
    Point3 curent_pixel = Point3(haut_gauche);
    int i = 0;
    while(i < Pixel_h)
    {
        int j = 0;
        while (j < Pixel_w)
        {
            ret.push_back(Point3(curent_pixel));
            j++;
            curent_pixel = curent_pixel + Img_side * pixel_dist_w;
        }
        i++;
        curent_pixel = haut_gauche + Img_up * (pixel_dist_h * i * -1);
    }
    return ret;
}