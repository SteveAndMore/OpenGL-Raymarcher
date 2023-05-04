#include "scene.hpp"

std::unique_ptr<TextureElements> Uniform_Texture::GetElements(Point3 point)
{
    std::unique_ptr<TextureElements> ret = std::make_unique<TextureElements>();
    ret->Kd = Kd;
    ret->Ks = Ks;
    ret->color =color;
    return ret;
}

//rayracer/
std::unique_ptr<Point3> Sphere::intersection(Line line)
{
    float n_x = (line.point.x - center.x);
    float n_y = (line.point.y - center.y);
    float n_z = (line.point.z - center.z);

    float a = line.dir.x * line.dir.x + line.dir.y * line.dir.y + line.dir.z * line.dir.z;

    float b  = 2 * line.dir.x * n_x;
          b += 2 * line.dir.y * n_y;
          b += 2 * line.dir.z * n_z;

    float c  = n_x * n_x;
          c += n_y * n_y;
          c += n_z * n_z;
          c -= radius * radius;

    auto solutions = Resolvequadra(a, b, c);

    int i = 0;
    while (i < solutions.size())
    {
        if (solutions[i] < 0.00001)
            solutions.erase(solutions.begin() + i);
        else
            i++;
    }
    if (solutions.size() == 0)
        return std::unique_ptr<Point3> (nullptr);
    else if(solutions.size() == 1)
    {
        return std::unique_ptr<Point3> (new Point3(line.point + line.dir * solutions[0]));
    }
    if (solutions[0] < solutions[1])
    {
        return std::unique_ptr<Point3> (new Point3(line.point + line.dir * solutions[0]));
    }
    if (solutions[1] < solutions[0])
    {
        return std::unique_ptr<Point3> (new Point3(line.point + line.dir * solutions[1]));
    }
    return std::unique_ptr<Point3> (nullptr);
}
Vector3 Sphere::GetNormal(Point3 point)
{
    return Vector3(center, point);
}
std::unique_ptr<RayHit> Scene::CastRay(Line ray)
{
    std::unique_ptr<Point3> closest_hit = nullptr;
    int object_hit_indice = 0;
    int i = 0;
    for (auto &obj : objects)
    {
        std::unique_ptr<Point3> point = std::move(obj->intersection(ray));
        if (point != nullptr)
        {
            if (closest_hit == nullptr || Vector3(ray.point, *closest_hit).norm() > Vector3(ray.point, *point).norm())
            {
                closest_hit = std::move(point);
                object_hit_indice = i;
            }
        }
        i++;
    }
    if (closest_hit == nullptr)
        return nullptr;
    else
    {
        std::unique_ptr<RayHit> ret = std::make_unique<RayHit>();
        ret->object_indice = object_hit_indice;
        ret->point = std::move(closest_hit);
        ret->origin = std::make_unique<Point3>(ray.point);
        return ret;
    }
        
}

//raymarcher/
float Sphere::dist(Point3 origin)
{
    float dist = Vector3(origin, center).norm() - radius;
    if (dist >= 0)
        return dist;
    return -dist;
}

std::unique_ptr<RayHit> Scene::MarchRay(Line ray)
{
    ray.dir = ray.dir.normalize();
    float dist = 0;
    Point3 point = ray.point;
    while(MAX_DIST > dist)
    {
        int i = 0;
        for (auto &obj : objects)
        {
            if (i == 0)
                dist = obj->dist(point);
            else
            {
                float dist_tmp = obj->dist(point);
                if (dist_tmp < dist)
                    dist = dist_tmp;
            }
            if (MIN_DIST >= dist)
            {
                std::unique_ptr<RayHit> ret = std::make_unique<RayHit>();
                ret->object_indice = i;
                ret->point = std::make_unique<Point3>(ray.point);
                ret->origin = std::make_unique<Point3>(point);
                return ret;
            }
            i++;
        }
        point = point + (ray.dir * dist);
    }
    return nullptr; //didn't hit anything
}

float pow (float nb, int p)
{
    if (p == 0)
        return 1;
    bool inv = false;
    if (p < 0)
    {
        p = -p;
        inv = true;
    }
    int i = 1;
    float ret = nb;
    while (i < p)
    {
        ret = ret * nb;
        i++;
    }
    if (inv)
        return 1/ret;
    return ret;
}

ColorRGB Scene::GetIllumination (std::unique_ptr<RayHit> ray_hit)
{
    ColorRGB ret = ColorRGB(0, 0, 0);
    if (ray_hit == nullptr)
        return ret;
    for (auto &light : lights)
    {
        std::unique_ptr<RayHit> light_hit = std::move(CastRay(Line(*ray_hit->point, light->pos)));
        Vector3 L = Vector3(*ray_hit->point, light->pos);
        if (light_hit != nullptr && (Vector3(*ray_hit->point, *light_hit->point).norm() < L.norm()))
        {
            continue;
        }
        auto texture_elements = objects[ray_hit->object_indice]->GetTextureElements(*ray_hit->point);
        auto N = objects[ray_hit->object_indice]->GetNormal(*ray_hit->point);
        auto S = Symetric(*ray_hit->origin, Line(*ray_hit->point, N));

        auto scal_NL = (L.normalize().scalaire(N.normalize()));
        auto scal_SL = (L.normalize().scalaire(S.normalize()));
        if (scal_SL < 0)
            scal_SL = 0;
        if (scal_NL < 0)
            scal_NL = 0;

        auto comp_diffuse = scal_NL * texture_elements->Kd * light->I;
        auto comp_speculaire = light->I * texture_elements->Ks * pow(scal_SL, ns);
        ColorRGB color;
/*
        if (Reflect > 0)
        {
            Reflect--;
            color = reflect(S);
        }
        else
            color = texture_elements->color;
*/
        
        ret = light->color * (color * (comp_diffuse + comp_speculaire));
    }
    return ret;
}