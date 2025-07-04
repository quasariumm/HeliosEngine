#ifndef RAY_GLSL
#define RAY_GLSL

/*
	Very basic file to just store the ray struct
*/

#include "/Engine/Raytracing/materialStruct.glsl"

struct RayHitInfo
{
	bool didHit;
	float dst;
	vec3 hitPoint;
	vec3 normal;
	vec3 tangent;
	vec3 lightVector;
	RayTracingMaterial material;
};

const RayHitInfo defaultHitInfo = RayHitInfo( false, 1e30, vec3(0.0), vec3(0.0), vec3(0.0), vec3(0.0), defaultMaterial );

struct Ray
{
	vec3 origin;
	vec3 dir;
	vec3 invDir;
	RayHitInfo hit;
};

#endif // RAY_GLSL