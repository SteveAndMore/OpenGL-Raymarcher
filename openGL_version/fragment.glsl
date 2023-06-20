#version 450
layout (location = 0) out vec4 fragColor;

uniform vec2 u_resolution = vec2(1024, 1024);
uniform vec3 cam_pos;
uniform vec3 cam_rot;
uniform float sceneType; //0 = basic, 1 = stylish, 2 = fractal
uniform float time;

const float FOV = 1.0;
const int MAX_STEPS = 256;
const float MAX_DIST = 500;
const float EPSILON = 0.001;

mat3 getCam(vec3 ro, vec3 lookAt) {
    vec3 camF = normalize(vec3(lookAt - ro));
    vec3 camR = normalize(cross(vec3(0, 1, 0), camF));
    vec3 camU = cross(camF, camR);
    return mat3(camR, camU, camF);
}

float fSphere(vec3 p, float r) {
    return length(p) - r;
}

float fPlane(vec3 p, vec3 n, float distanceFromOrigin) {
    return dot(p, n) + distanceFromOrigin;
}

vec2 fOpUnionID(vec2 res1, vec2 res2) {
    return (res1.x < res2.x) ? res1 : res2;
}

float fDisplace(vec3 p) {
    float time_ = time / 1000;
    float h = sin(p.y + sin(5.0 * time_));
    if (h < -0.1)
        h = -0.1;
    return (sin(p.x + time_ * 5) + 1) * (sin(p.z +  0.1 * sin(time_ * 10)) - 1) * h;
}

vec2 map(vec3 p) {
    // shere
    if (sceneType == 2)
        p = mod(p ,10);
    vec3 ps = p - vec3(5.0, 5.0, 5.0);
    float sphereDist = fSphere(ps, 0.6);
    float sphereID = 1.0;
    vec2 sphere = vec2(sphereDist, sphereID);
    // plane
    float planeDist = fPlane(p, vec3(0, 1, 0), 4.0 + fDisplace(p));
    float planeID = 2.0;
    vec2 plane = vec2(planeDist, planeID);

    vec2 ret = fOpUnionID(sphere, plane);
    return ret;
}

vec3 getNormal(vec3 p) {
    vec2 e = vec2(EPSILON, 0.0);
    vec3 n = vec3(map(p).x) - vec3(map(p - e.xyy).x, map(p - e.yxy).x, map(p - e.yyx).x);
    return normalize(n);
}

vec2 rayMarch(vec3 origin, vec3 dir) {
    vec2 hit;
    float dist_total = 0.0;
    for (int i = 0; i < MAX_STEPS; i++) {
        vec3 p = origin + dir * dist_total;
        hit = map(p);
        dist_total += hit.x;
        if (abs(hit.x) < EPSILON || dist_total > MAX_DIST) break;
    }
    return vec2(dist_total, hit.y);
}

bool lightMarch(vec3 origin, vec3 dir, float dist) {
    vec2 hit;
    float dist_total = 0.0;
    for (int i = 0; i < MAX_STEPS; i++) {
        vec3 p = origin + dir * dist_total;
        hit = map(p);
        dist_total += hit.x;
        if (abs(hit.x) < EPSILON || dist_total > MAX_DIST || dist_total > dist)
            break;
    }
    return dist_total < dist;
}

vec3 getLight(vec3 p, vec3 dir, vec3 color)
{
    if (sceneType == 2)
    {
        vec3 norm = getNormal(p);
        return norm * color;
    }
    vec3 lightPos = vec3(7.0, 7.0, -7.0);
    vec3 light  = normalize(lightPos - p);
    vec3 norm = getNormal(p);
    vec3 spec_reflect = reflect(-light, norm);


    vec3 specular = vec3(0.6) * pow(clamp(dot(spec_reflect, -dir), 0.0, 1.0), 10.0);
    vec3 diffuse = color * clamp(dot(light, norm), 0.0, 1.0);

    if (lightMarch(p + norm * 0.02, normalize(lightPos), length(lightPos - p)))
       return specular;
    return diffuse + specular;
}

void render(inout vec3 col, in vec2 uv)
{
    vec3 origin = cam_pos;
    vec3 lookAt = normalize(cam_rot);

    vec3 dir = getCam(vec3(0.0,0.0,0.0), lookAt) * normalize(vec3(uv, FOV));

    vec2 hit = rayMarch(origin, dir);
    if (abs(hit.x) < MAX_DIST) {
        vec3 p = origin + dir * hit.x;
        if (hit.y == 1)
            col = getLight(p, dir, vec3(1,0.8,0.5));
        else
            col = getLight(p, dir, vec3(0,1,0));
    }
    else
        col = vec3(0,0,0);
}

void main()
{
    vec2 uv = (2.0 * gl_FragCoord.xy - u_resolution.xy) / u_resolution.y;

    vec3 col = vec3(0,0,0);
    render(col, uv);

    fragColor = vec4(col, 1.0);
}
