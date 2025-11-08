#ifndef RAY_GLSL
#define RAY_GLSL

/*
	Very basic file to just store the ray struct
*/

#include "/Engine/raytracing/materialStruct.glsl"

struct RayHitInfo
{
	// Flags
	bool didHit;
	bool inside;

	// Hit point info
	vec3 hitPoint;
	float dst;

	// The surface normal
	vec3 normal;

	// Index into the materials list
	int materialIndex;

	// Tangent
	vec4 tangent;

	// Light direction
	vec3 lightVector;

	// entt hit info
	uint entity;

	// Texture data
	vec2 barycentrics;
	vec2 texcoords;
};

const RayHitInfo defaultHitInfo = RayHitInfo( false, false, vec3(0.0), 1e30, vec3(0.0), 0, vec4(0.0), vec3(0.0), 4294967295, vec2(0.0), vec2(0.0) );

struct Ray
{
	vec3 origin;
	vec3 dir;
	vec3 invDir;
	RayHitInfo hit;
};

#endif // RAY_GLSL