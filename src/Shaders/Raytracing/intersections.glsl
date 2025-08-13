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
	int materialIndex;
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
		ray.hit.materialIndex = sphere.materialIndex;
		ray.hit.dst = t1;
		ray.hit.hitPoint = ray.origin + ray.dir * t1;
		ray.hit.normal = normalize(ray.hit.hitPoint - sphere.center);

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
		ray.hit.materialIndex = sphere.materialIndex;
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
	// The normal is in the first 16 bits of an axis, tangent in the other 16 bits
	// This needs to be a uint because of parameter reasons
	float texCoordX;
	uvec3 normalTanget;
	// Bitangent is cross(normal, tangent)
	float texCoordY;
};

void UnpackNormalTangent(in uvec3 original, inout vec3 normal, inout vec3 tangent)
{
	for (int i = 0; i < 3; ++i)
	{
		vec2 res = unpackHalf2x16(original[i]);
		normal[i] = res[0];
		tangent[i] = res[1];
	}
}

uniform uint NumVertices = 0;
layout (std430, binding = 5) readonly buffer VertexBuffer
{
	Vertex Vertices[];
};

uniform uint NumIndices = 0;
layout (std430, binding = 6) readonly buffer IndicesBuffer
{
	uint Indices[];
};

struct Mesh
{
	uint indexCount;
	uint firstIndex;
	vec3 position;
	int materialIndex;
};

uniform uint NumMeshes = 0;
layout (std430, binding = 7) readonly buffer MeshBuffer
{
	Mesh Meshes[];
};

void RayTriangle(inout Ray ray, Mesh mesh, Vertex v0, Vertex v1, Vertex v2)
{
	// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm#C++_implementation
	vec3 edge1 = v1.position - v0.position;
	vec3 edge2 = v2.position - v0.position;
	vec3 ray_cross_e2 = cross(ray.dir, edge2);
	float det = dot(edge1, ray_cross_e2);

	if (abs(det) < 0.00001) return;

	float inv_det = 1.0 / det;
	vec3 s = ray.origin - (v0.position /* + mesh.position */);
	float u = inv_det * dot(s, ray_cross_e2);
	if (u < 0.0 || u > 1.0) return;
	vec3 s_cross_e1 = cross(s, edge1);
	float v = inv_det * dot(ray.dir, s_cross_e1);
	if (v < 0.0 || v + u > 1.0) return;
	float t = inv_det * dot(edge2, s_cross_e1);

	if (t > 0.0 && t < ray.hit.dst)
	{
		ray.hit.didHit = true;
		ray.hit.materialIndex = mesh.materialIndex;
		ray.hit.dst = t;
		ray.hit.hitPoint = ray.origin + ray.dir * t;
		ray.hit.barycentrics = vec2(u, v);
		float w = 1.0 - u - v;

		// Sorry for a bit of Jacco code here
		vec3 v0n = vec3(0.0); vec3 v0t = vec3(0.0); UnpackNormalTangent(v0.normalTanget, v0n, v0t);
		vec3 v1n = vec3(0.0); vec3 v1t = vec3(0.0); UnpackNormalTangent(v1.normalTanget, v1n, v1t);
		vec3 v2n = vec3(0.0); vec3 v2t = vec3(0.0); UnpackNormalTangent(v2.normalTanget, v2n, v2t);

		ray.hit.normal = u * v1n + v * v2n + w * v0n;
		ray.hit.tangent = u * v1t + v * v2t + w * v0t;
	}
}

void RayMesh(inout Ray ray, Mesh mesh)
{
	// Check triangles based on the indices
	for (uint i = mesh.firstIndex; i < mesh.firstIndex + mesh.indexCount; i += 3)
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

void RayCollision(inout Ray ray)
{
	ray.hit = defaultHitInfo;
	ray.hit.didHit = false;
	ray.hit.dst = 1e30;
	for (int i = 0; i < NumSpheres; ++i)
		RaySphere(ray, Spheres[i]);

	for (uint i = 0; i < NumMeshes; ++i)
		RayMesh(ray, Meshes[i]);
}

#endif // INTERSECTIONS_GLSL