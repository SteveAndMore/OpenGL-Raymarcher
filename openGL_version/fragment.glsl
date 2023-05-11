#version 450
layout (location = 0) out vec4 fragColor;

uniform vec2 u_resolution = vec2(1024, 1024);

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

vec2 map(vec3 p) {
    vec3 ps = vec3(0.0, 0.0, 3.0) - p;
    float sphereDist = fSphere(ps, 0.6);
    float sphereID = 1.0;
    vec2 sphere = vec2(sphereDist, sphereID);
    return sphere;
}

vec3 getNormal(vec3 p) {
    vec2 e = vec2(EPSILON, 0.0);
    vec3 n = vec3(map(p).x) - vec3(map(p - e.xyy).x, map(p - e.yxy).x, map(p - e.yyx).x);
    return normalize(n);
}

vec3 getLight(vec3 p, vec3 dir, vec3 color)
{
    vec3 norm = getNormal(p);
    vec3 fresnel = 0.9 - (color * pow(1.0 + dot(dir, norm), 3.0));
    return fresnel;
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

void render(inout vec3 col, in vec2 uv)
{
    vec3 origin = vec3(0.0, 0.0, -1.0);
    vec3 lookAt = vec3(0, 0, 1);

    vec3 dir = getCam(origin, lookAt) * normalize(vec3(uv, FOV));

    vec2 hit = rayMarch(origin, dir);
    if (abs(hit.x) < MAX_DIST) {
        vec3 p = origin + dir * hit.x;
        col = getLight(p, dir, vec3(1,1,1));
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
