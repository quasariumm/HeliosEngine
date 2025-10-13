#ifndef RAY_CL_
#define RAY_CL_

struct HitInfo
{
    bool hit;
    bool inside;
    float dst;
    float3 hitPoint;
    float3 normal;
    float3 tangent;
    float3 lightVector;
    float2 barycentric;
    int materialIndex;
};

struct Ray
{
    float3 origin;
    float3 dir;
    float3 invDir;
    struct HitInfo hit;
};

#endif