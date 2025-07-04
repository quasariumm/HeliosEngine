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
	// Thanks for the code, Sebastian Lague
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
		ray.hit.didHit = true;
		ray.hit.material = sphere.material;
		ray.hit.dst = t1;
		ray.hit.hitPoint = ray.origin + ray.dir * t1;
		ray.hit.normal = normalize(hitInfo.hitPoint - sphere.center);

		// Calculate tangent vector
		vec3 arbitraryDirection = vec3(1.0, 0.0, 0.0); // Choose an arbitrary direction
		if (dot(ray.hit.normal, arbitraryDirection) > 0.99) // Check if it's too close to the normal
		arbitraryDirection = vec3(0.0, 1.0, 0.0); // Choose another direction

		ray.hit.tangent = normalize(cross(ray.hit.normal, arbitraryDirection));
		return;
	}

	if (t2 >= 0.0 && t2 < ray.hit.dst && t1 <= 0)
	{
		ray.hit.didHit = true;
		ray.hit.material = sphere.material;
		ray.hit.dst = t2;
		ray.hit.hitPoint = ray.origin + ray.dir * t2;
		ray.hit.normal = normalize(ray.hit.hitPoint - sphere.center);

		// Calculate tangent vector
		vec3 arbitraryDirection = vec3(1.0, 0.0, 0.0); // Choose an arbitrary direction
		if (dot(ray.hit.normal, arbitraryDirection) > 0.99) // Check if it's too close to the normal
		arbitraryDirection = vec3(0.0, 1.0, 0.0); // Choose another direction

		ray.hit.tangent = normalize(cross(ray.hit.normal, arbitraryDirection));
		return;
	}
}

/*
	Triangles
*/

struct Vertex
{
	vec3 position;
	vec3 normal;
	vec3 tangent;
	// Bitangent is cross(normal, tangent)
};

layout (std430, binding = 5) readonly buffer VertexBuffer
{
	int NumVertices;
	Vertex Vertices[];
};

layout (std430, binding = 6) readonly buffer IndicesBuffer
{
	int NumIndices;
	int Indices[];
};

struct Mesh
{
	int IndexCount;
	int FirstIndex;
	RayTracingMaterial material;
};

layout (std430, binding = 7) readonly buffer MeshBuffer
{
	int NumMeshes;
	Mesh Meshes[];
};

void RayTriangle(inout Ray ray, Mesh mesh, Vertex v0, Vertex v1, Vertex v2)
{
	vec3 edge1 = v1.position - v0.position;
	vec3 edge2 = v2.position - v0.position;
	vec3 h = cross(ray.dir, edge2);
	float a = dot(edge1, h);

	if (abs(a) < 0.00001) return;

	float f = 1.0 / a;
	vec3 s = ray.origin - v0.position;
	float u = f * dot(s, h);
	if (u < 0.0 || u > 1.0) return;
	vec3 q = cross(s, edge1);
	float v = f * dot(ray.dir, q);
	if (v < 0.0 || v + u > 1.0) return;
	float t = f * dot(edge2, q);

	if (t > 0.0 && t < ray.hit.dst)
	{
		ray.hit.didHit = true;
		ray.hit.material = mesh.material;
		ray.hit.dst = t;
		ray.hit.hitPoint = ray.origin + ray.dir * t2;
		float w = 1.0 - u - v;
		ray.hit.normal = u * v0.normal + v * v1.normal + w * v2.normal;
		ray.hit.tangent = u * v0.tangent + v * v1.tangent + w * v2.tangent;
	}
}

void RayMesh(inout Ray ray, Mesh mesh)
{
	// Check triangles based on the indices
	for (int i = mesh.FirstIndex; i < mesh.FirstIndex + mesh.IndexCount; i += 3)
	{
		RayTriangle(
			ray,
			mesh,
			Vertices[Indices[i]],
			Vertices[Indices[i + 1]],
			Vertices[Indices[i + 2]]
		);
	}
}

/*
	Scene collision
*/

void RayCollision(Ray ray)
{
	ray.hit = defaultHitInfo;
	ray.hit.didHit = false;
	ray.hit.dst = 1e30;
	for (int i = 0; i < NumSpheres; ++i)
		RaySphere(ray, Spheres[i]);

	for (int i = 0; i < NumMeshes; ++i)
		RayMesh(ray, Meshes[i]);
}

#endif // INTERSECTIONS_GLSL