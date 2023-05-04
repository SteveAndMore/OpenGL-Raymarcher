#include "scene.hpp"
#include "image.hpp"
#include <memory>
#include <iomanip>

void RayTrace(std::unique_ptr<Scene> scene)
{
    std::vector<Point3> pixeles_pos = scene->camera->GetPixelPos();
    Image img = Image(IMG_W, IMG_H);
    int i = 0;
    for (auto& pixel : pixeles_pos)
    {
        auto ray_hit = scene->CastRay(Line(scene->camera->center, pixel));
        img.pixels[i].color = scene->GetIllumination(std::move(ray_hit));
        i++;
    }
    img.ToPPM("RayTrace_img.ppm");
}

void RayMarch(std::unique_ptr<Scene> scene)
{
    std::vector<Point3> pixeles_pos = scene->camera->GetPixelPos();
    Image img = Image(IMG_W, IMG_H);
    //float dist = scene->Get_min_dist(scene->camera->center);
    int i = 0;
    for (auto& pixel : pixeles_pos)
    {
        auto ray_hit = scene->MarchRay(Line(scene->camera->center, pixel));
        if (ray_hit == nullptr)
            img.pixels[i].color = ColorRGB(0, 0, 0);
        else
            img.pixels[i].color = scene->objects[ray_hit->object_indice]->GetTextureElements(Point3(std::move(ray_hit->point)))->color;

        i++;
    }
    img.ToPPM("RayMarch_img.ppm");
}

int main (int argc, char const* argv[])
{
    std::unique_ptr<Scene> scene = std::make_unique<Scene>();

    std::unique_ptr<Uniform_Texture> RedTexture = std::make_unique<Uniform_Texture>(255, 0, 0, 0.8, 3);
    std::unique_ptr<Sphere> s1 = std::make_unique<Sphere>(Point3(3,0,0), 1, std::move(RedTexture));

    std::unique_ptr<Uniform_Texture> texture2 = std::make_unique<Uniform_Texture>(20, 200, 40, 1, 1);
    std::unique_ptr<Sphere> s2 = std::make_unique<Sphere>(Point3(5,2,1), 1, std::move(texture2));
    
    
    std::unique_ptr<Point_Light> p_l = std::make_unique<Point_Light>(1, 1, 1, ColorRGB(255, 255, 255), 1);
    scene->addObject(std::move(s1));
    scene->addObject(std::move(s2));
    scene->addCamera(Point3(0,0,0), Point3(1,0,0), Vector3(0,1,0), 45, 45, 1);
    scene->addLight(std::move(p_l));
    RayMarch(std::move(scene));
    
    return 0;
}