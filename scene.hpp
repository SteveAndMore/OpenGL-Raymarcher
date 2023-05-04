#pragma once
#include <memory>
#include <utility>
#include "maths.hpp"
#include "vision.hpp"

#define ns 4
#define MAX_DIST 1000.0
#define MIN_DIST 0.01

struct TextureElements
{
    float Kd;
    float Ks;
    ColorRGB color;
};

class Texture_Material
{
public:
    virtual std::unique_ptr<TextureElements> GetElements(Point3 point)=0;
    virtual ~Texture_Material() = default;
};

class Uniform_Texture : public Texture_Material
{
public:
    ColorRGB color;
    float Kd;
    float Ks;
    Uniform_Texture(ColorRGB color, float Kd, float Ks):color(color), Ks(Ks), Kd(Kd){}
    Uniform_Texture(u_int8_t r, u_int8_t g, u_int8_t b, float Kd, float Ks):color(ColorRGB(r, g, b)), Ks(Ks), Kd(Kd){}

    std::unique_ptr<TextureElements> GetElements(Point3 point);
};

class Object
{
public:
    std::unique_ptr<Texture_Material> texture = nullptr;
//raytracer
    virtual std::unique_ptr<Point3> intersection(Line line)=0;
    virtual Vector3 GetNormal(Point3 point)=0;
//
//raymacher
    virtual float dist(Point3 point)=0;
//
    virtual std::unique_ptr<TextureElements> GetTextureElements(Point3 point)=0;
    virtual ~Object() = default;
};

class Sphere : public Object
{
public:
    Point3 center;
    float radius;
    Sphere(Point3 center, float radius, std::unique_ptr<Texture_Material> texture):center(center), radius(radius)
    {
        this->texture = std::move(texture);
    }

//raytracer
    std::unique_ptr<Point3> intersection(Line line);
//raymarcher
    float dist(Point3 point);
//
    Vector3 GetNormal(Point3 point);
    std::unique_ptr<TextureElements> GetTextureElements(Point3 point) {return std::move(texture->GetElements(point));}
};

struct RayHit
{
    int object_indice;
    std::unique_ptr<Point3> point;
    std::unique_ptr<Point3> origin;
};

class Scene
{
public:
    std::vector<std::unique_ptr<Object>> objects;
    std::vector<std::unique_ptr<Light>> lights;
    std::unique_ptr<Cammera> camera;
    Scene(){}

    void addObject(std::unique_ptr<Object> obj){objects.push_back(std::move(obj));}
    void addLight(std::unique_ptr<Light> light){lights.push_back(std::move(light));}
    void addCamera(Point3 center, Point3 target, Vector3 up,float a, float b, float zmin)
    {
        camera = std::unique_ptr<Cammera>(new Cammera(center, target, up, a, b, zmin));
    }
//raymarcher
    std::unique_ptr<RayHit> MarchRay(Line ray);
//raytracer
    std::unique_ptr<RayHit> CastRay(Line ray);
    ColorRGB GetIllumination (std::unique_ptr<RayHit> ray_hit);
};