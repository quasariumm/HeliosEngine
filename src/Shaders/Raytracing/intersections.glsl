#ifndef INTERSECTIONS_GLSL
#define INTERSECTIONS_GLSL

#include "/Engine/Raytracing/ray.glsl"
#include "/Engine/Raytracing/materials.glsl"

/*
	Holds the primitve structs and the ray-primitive intersection logic
*/

struct Sphere
{
	vec3 center;
	float radius;
	RayTracingMaterial material;
};
#define SPHERES_MAX 16
uniform Sphere Spheres[SPHERES_MAX];
uniform int NumSpheres = 0;

void RaySphere(inout Ray ray, Sphere sphere)
{
	sphere.radius = 1;
	
	// Thanks for the code, Sebastian Lague
	RayHitInfo hitInfo = defaultHitInfo;
	hitInfo.material = sphere.material;

	vec3 oc = ray.origin - sphere.center;
	float b = dot(oc, ray.dir);
	float d = b * b - (dot(oc, oc) - sphere.radius * sphere.radius);

	if (d <= 0.0)
		return;

	float sqrt_d = sqrt(d);
	float t1 = -b - sqrt_d;
	float t2 = -b + sqrt_d;

	if (t1 >= 0.0 && t1 < ray.hit.dst)
	{
		hitInfo.didHit = true;
		hitInfo.dst = t1;
		hitInfo.hitPoint = ray.origin + ray.dir * t1;
		hitInfo.normal = normalize(hitInfo.hitPoint - sphere.center);

		// Calculate tangent vector
		vec3 arbitraryDirection = vec3(1.0, 0.0, 0.0); // Choose an arbitrary direction
		if (dot(hitInfo.normal, arbitraryDirection) > 0.99) // Check if it's too close to the normal
		arbitraryDirection = vec3(0.0, 1.0, 0.0); // Choose another direction

		hitInfo.tangent = normalize(cross(hitInfo.normal, arbitraryDirection));

		ray.hit = hitInfo;
		return;
	}

	if (t2 >= 0.0 && t2 < ray.hit.dst && t1 <= 0)
	{
		hitInfo.didHit = true;
		hitInfo.dst = t2;
		hitInfo.hitPoint = ray.origin + ray.dir * t2;
		hitInfo.normal = normalize(hitInfo.hitPoint - sphere.center);

		// Calculate tangent vector
		vec3 arbitraryDirection = vec3(1.0, 0.0, 0.0); // Choose an arbitrary direction
		if (dot(hitInfo.normal, arbitraryDirection) > 0.99) // Check if it's too close to the normal
		arbitraryDirection = vec3(0.0, 1.0, 0.0); // Choose another direction

		hitInfo.tangent = normalize(cross(hitInfo.normal, arbitraryDirection));

		ray.hit = hitInfo;
		return;
	}
}

void RayCollision(inout Ray ray)
{
	ray.hit = defaultHitInfo;
	ray.hit.didHit = false;
	ray.hit.dst = 1e30;
	for (int i = 0; i < NumSpheres; ++i)
		RaySphere(ray, Spheres[i]);
}

#endif // INTERSECTIONS_GLSL