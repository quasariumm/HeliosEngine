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

RayHitInfo RaySphere(Ray ray, Sphere sphere)
{
	// Thanks for the code, Sebastian Lague
	RayHitInfo hitInfo = defaultHitInfo;
	hitInfo.material = sphere.material;

	vec3 difference = sphere.center - ray.origin;
	float a = dot(ray.dir, ray.dir);
	float b = -2.0 * dot(difference, ray.dir);
	float c = dot(difference, difference) - sphere.radius * sphere.radius;
	float disc = b*b - 4.0*a*c;
	if (disc < 0.0)
	{
		// Ray truly misses the sphere
		return hitInfo;
	}

	float sqrtDisc = sqrt(disc);
	// Distance to nearest intersection point (from quadratic formula)
	float dst = (-b - sqrtDisc) / (2.0 * a);

	// Ignore intersections that occur behind the ray
	if (dst >= 0.0)
	{
		hitInfo.didHit = true;
		hitInfo.dst = dst;
		hitInfo.hitPoint = ray.origin + ray.dir * dst;
		hitInfo.normal = normalize(hitInfo.hitPoint - sphere.center);

		// Calculate tangent vector
		vec3 arbitraryDirection = vec3(1.0, 0.0, 0.0); // Choose an arbitrary direction
		if (dot(hitInfo.normal, arbitraryDirection) > 0.99) // Check if it's too close to the normal
		arbitraryDirection = vec3(0.0, 1.0, 0.0); // Choose another direction

		hitInfo.tangent = normalize(cross(hitInfo.normal, arbitraryDirection));
		return hitInfo;
	}

	float dst2 = (-b + sqrtDisc) / (2.0 * a);

	if (dst2 >= 0.0)
	{
		hitInfo.didHit = true;
		hitInfo.dst = dst2;
		hitInfo.hitPoint = ray.origin + ray.dir * dst2;
		hitInfo.normal = normalize(hitInfo.hitPoint - sphere.center);

		// Calculate tangent vector
		vec3 arbitraryDirection = vec3(1.0, 0.0, 0.0); // Choose an arbitrary direction
		if (dot(hitInfo.normal, arbitraryDirection) > 0.99) // Check if it's too close to the normal
		arbitraryDirection = vec3(0.0, 1.0, 0.0); // Choose another direction

		hitInfo.tangent = normalize(cross(hitInfo.normal, arbitraryDirection));
		return hitInfo;
	}

	return hitInfo;
}

RayHitInfo RayCollision(Ray ray)
{
	RayHitInfo closest = defaultHitInfo;
	closest.didHit = false;
	closest.dst = 1e30;
	for (int i = 0; i < NumSpheres; ++i)
	{
		RayHitInfo sphereInfo = RaySphere(ray, Spheres[i]);
		if (sphereInfo.didHit
		&& (sphereInfo.dst < closest.dst))
		{
			closest = sphereInfo;
		}
	}
	return closest;
}

#endif // INTERSECTIONS_GLSL