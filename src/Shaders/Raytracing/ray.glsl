#ifndef RAY_GLSL
#define RAY_GLSL

/*
	Very basic file to just store the ray struct
*/

struct Ray
{
	vec3 origin;
	vec3 dir;
	vec3 invDir;
};

#endif // RAY_GLSL