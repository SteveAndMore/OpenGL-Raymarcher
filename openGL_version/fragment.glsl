#version 450
layout (location = 0) out vec4 fragColor;

uniform vec2 u_resolution;
uniform vec3 cam_pos;
uniform vec3 cam_rot;
uniform float sceneType; //0 = basic, 1 = stylish, 2 = fractal
uniform float time;
uniform float canon_time = -1;

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

float fBox(vec3 p, vec3 b) {
    vec3 q = abs(p) - b;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

vec2 fOpUnionID(vec2 res1, vec2 res2) {
    return (res1.x < res2.x) ? res1 : res2;
}

vec2 fOpDifferenceID(vec2 res1, vec2 res2) {
    return (res1.x > -res2.x) ? res1 : vec2(-res2.x, res2.y);
}
float fOpSmoothUnion( float d1, float d2, float k ) {
    float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) - k*h*(1.0-h);
}

float wave(vec3 p) {
    float time_ = time / 1000;
    float h = sin(time_ * 3);
    return (sin(p.x + time_ * 5.)) * h;
}
float waveFlag(vec3 p) {
    float time_ = time / 3000;
    return -abs((sin(p.x + time_ * 5.)) * 0.1);
}

float fCylinder( vec3 p, vec3 a, vec3 b, float r )
{
  vec3  ba = b - a;
  vec3  pa = p - a;
  float baba = dot(ba,ba);
  float paba = dot(pa,ba);
  float x = length(pa*baba-ba*paba) - r*baba;
  float y = abs(paba-baba*0.5)-baba*0.5;
  float x2 = x*x;
  float y2 = y*y*baba;
  float d = (max(x,y)<0.0)?-min(x2,y2):(((x>0.0)?x2:0.0)+((y>0.0)?y2:0.0));
  return sign(d)*sqrt(abs(d))/baba;
}

float dot2( in vec3 v ) { return dot(v,v); }

float fQuad( vec3 p, vec3 a, vec3 b, vec3 c, vec3 d )
{
  vec3 ba = b - a; vec3 pa = p - a;
  vec3 cb = c - b; vec3 pb = p - b;
  vec3 dc = d - c; vec3 pc = p - c;
  vec3 ad = a - d; vec3 pd = p - d;
  vec3 nor = cross( ba, ad );

  return sqrt(
    (sign(dot(cross(ba,nor),pa)) +
     sign(dot(cross(cb,nor),pb)) +
     sign(dot(cross(dc,nor),pc)) +
     sign(dot(cross(ad,nor),pd))<3.0)
     ?
     min( min( min(
     dot2(ba*clamp(dot(ba,pa)/dot2(ba),0.0,1.0)-pa),
     dot2(cb*clamp(dot(cb,pb)/dot2(cb),0.0,1.0)-pb) ),
     dot2(dc*clamp(dot(dc,pc)/dot2(dc),0.0,1.0)-pc) ),
     dot2(ad*clamp(dot(ad,pd)/dot2(ad),0.0,1.0)-pd) )
     :
     dot(nor,pa)*dot(nor,pa)/dot2(nor) );
}

vec2 map(vec3 p) {
    if (sceneType == 2)
    {
        p = mod(p ,10);
        vec3 ps = p - vec3(5.0, 5.0, 5.0);
        float sphereDist = fSphere(ps, 0.6);
        float sphereID = 1.0;
        return vec2(sphereDist, sphereID);
    }

    if (sceneType == 1)
    {
        vec3 boatCenter = vec3(0,-5, 20);
        // cylindre
        float cylinderDist = fCylinder(boatCenter - p, vec3(10,0,0), vec3(-10, 0,0), 3.5);
        float cylinderID = 12.0;
        vec2 cylinder = vec2(cylinderDist, cylinderID);
        // cut boat box
        float cutBoatBoxDist = fBox(boatCenter - p + vec3(0, 5, 0), vec3(20, 5, 5));
        float cutBoatBoxID = 13.0;
        vec2 cutBoatBox = vec2(cutBoatBoxDist, cutBoatBoxID);
        // sphere
        float sphereDist = fSphere(boatCenter - p + vec3(10, 0, 0), 3.5);
        float sphereID = 12.0;
        vec2 sphere = vec2(sphereDist, sphereID);
        // sphere_2
        float sphereDist2 = fSphere(boatCenter - p + vec3(-10, 0, 0), 3.5);
        float sphereID2 = 12.0;
        vec2 sphere2 = vec2(sphereDist2, sphereID2);
        //Mat
        float matDist = fCylinder(boatCenter - p + vec3(0, 10, 0), vec3(0,20,0), vec3(0, 0, 0), 0.3);
        float matID = 13.0;
        vec2 mat = vec2(matDist, matID);
        //flag
        float flagDist = fQuad(boatCenter - p + vec3(2, 8.4, 0) , vec3(2, -1,0), vec3(-2,-1,0), vec3(-2,1,0), vec3(2,1,0));
        flagDist += waveFlag(p);
        float flagID = 14.0;
        vec2 flag = vec2(flagDist, flagID);

        //canon
        vec3 canonCenter = boatCenter + vec3(3, 1, -4);
        float canonDist = fCylinder(canonCenter - p, vec3(0,0,1.5), vec3(0, 0,-1.5), 0.3);
        float canonID = 15.0;
        vec2 canon = vec2(canonDist, canonID);
        //canonInt
        float canonIntDist = fCylinder(canonCenter - p, vec3(0,0,1.6), vec3(0, 0,-1.5), 0.2);
        float canonIntID = 16.0;
        vec2 canonInt = vec2(canonIntDist, canonIntID);
        //weel1
        float weel1Dist = fCylinder(canonCenter - p + vec3(0.5, -0.5, 1), vec3(0.2,0,0), vec3(0, 0,0), 0.5);
        vec2 weel1 = vec2(weel1Dist, canonID);
        //weel2
        float weel2Dist = fCylinder(canonCenter - p + vec3(-0.5, -0.5, 1), vec3(-0.2,0,0), vec3(0, 0,0), 0.5);
        vec2 weel2 = vec2(weel2Dist, canonID);
        //bullet
        vec3 bullet_pos;
        if (canon_time > 0)
        {
            bullet_pos = vec3(0, 0,1 - (time - canon_time)/50);
        }
        float bulletDist = fSphere(canonCenter - p + bullet_pos, 0.2);
        vec2 bullet = vec2(bulletDist, canonID);

        // water
        float waterDist = fPlane(p, vec3(0, 1, 0), 7.0 + wave(p));
        float waterID = 11.0;
        vec2 water = vec2(waterDist, waterID);

        vec2 ret;
        ret = fOpUnionID(sphere, cylinder);
        ret = fOpUnionID(sphere2, ret);
        ret = fOpDifferenceID(ret, cutBoatBox);
        ret = fOpUnionID(mat, ret);
        ret = fOpUnionID(flag, ret);
        canon = fOpDifferenceID(canon, canonInt);
        if (canon_time > 0)
            canon.x = fOpSmoothUnion(canon.x, bullet.x, 1);
        ret = fOpUnionID(canon, ret);
        ret = fOpUnionID(ret, weel1);
        ret = fOpUnionID(ret, weel2);
        ret = fOpUnionID(water, ret);
        return ret;
    }
    vec2 ret;
    // cube
    float boxDist = fBox(p - vec3(0.2, 0.2, 1.5), vec3(0.3, 0.3, 0.3));
    float boxID = 3.0;
    vec2 box = vec2(boxDist, boxID);


    // shere
    vec3 ps = p - vec3(0.2, 0.2, 1.5);
    float sphereDist = fSphere(ps, 0.4);
    float sphereID = 1.0;
    vec2 sphere = vec2(sphereDist, sphereID);


    // plane
    float planeDist = fPlane(p, vec3(0, 1, 0), 4.0);
    float planeID = 2.0;
    vec2 plane = vec2(planeDist, planeID);

    ret = fOpDifferenceID(box, sphere);
    ret = fOpUnionID(ret, plane);
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
    vec3 ambient = color * 0.05;


    vec3 specular = vec3(0.6) * pow(clamp(dot(spec_reflect, -dir), 0.0, 1.0), 10.0);
    vec3 diffuse = color * clamp(dot(light, norm), 0.0, 1.0);

    if (lightMarch(p + norm * 0.02, normalize(lightPos), length(lightPos - p)))
       return specular + ambient;
    return diffuse + specular + ambient;
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
        else if (hit.y == 2)
            col = getLight(p, dir, vec3(0.6,0.6,0.2));
        else if (hit.y == 3)
            col = getLight(p, dir, vec3(0.3,0.2,0.4));
        else if (hit.y == 11)
            col = getLight(p, dir, vec3(0.1,0.3,0.8));
        else if (hit.y == 12)
            col = getLight(p, dir, vec3(0.9,0.67,0.5));
        else if (hit.y == 13)
            col = getLight(p, dir, vec3(1,0.9,0.8));
        else
            col = getLight(p, dir, vec3(0.5,0.5,0.5));
        if (sceneType == 1)
            col = mix(col, vec3(0.5, 0.7, 0.9), 1.0 - exp(-0.0001 * hit.x * hit.x));
    }
    else
    {
        if (sceneType == 1)
            col = vec3(0.5, 0.7, 0.9);
        else
            col = vec3(0,0,0);
    }
}

void main()
{
    vec2 uv = (2.0 * gl_FragCoord.xy - u_resolution.xy) / u_resolution.y;

    vec3 col = vec3(0,0,0);
    render(col, uv);

    fragColor = vec4(col, 1.0);
}
