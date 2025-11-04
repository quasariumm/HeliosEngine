#ifndef INTERSECTIONS_GLSL
#define INTERSECTIONS_GLSL

#include "/Engine/raytracing/ray.glsl"
#include "/Engine/raytracing/materials.glsl"

/*
	Holds the primitve structs and the ray-primitive intersection logic
*/

struct Sphere
{
	vec3 position;
	float radius;
	int materialIndex;
	uint entity;
	int padding[2];
};
uniform uint NumSpheres = 0;
layout (std430, binding = 3) readonly buffer SpheresBuffer
{
	Sphere Spheres[];
};

void RaySphere(inout Ray ray, Sphere sphere)
{
	// Thanks for the code, Sebastian Lague
	vec3 oc = ray.origin - sphere.position;
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
		ray.hit.inside = b < 0.0;
		ray.hit.materialIndex = sphere.materialIndex;
		ray.hit.entity = sphere.entity;
		ray.hit.dst = t1;
		ray.hit.hitPoint = ray.origin + ray.dir * t1;
		ray.hit.normal = normalize(ray.hit.hitPoint - sphere.position);

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
		ray.hit.inside = b < 0.0;
		ray.hit.materialIndex = sphere.materialIndex;
		ray.hit.entity = sphere.entity;
		ray.hit.dst = t2;
		ray.hit.hitPoint = ray.origin + ray.dir * t2;
		ray.hit.normal = normalize(ray.hit.hitPoint - sphere.position) * sign(b);

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
	vec3 position;
	int materialIndex;
	uint firstIndex;
	uint firstBvhNode;
	uint indexNodeCount; // Combines indexCount (left 2 bytes) and bvhNodeCount (right 2 bytes)
	uint entity;
};

uniform uint NumMeshes = 0;
layout (std430, binding = 7) readonly buffer MeshBuffer
{
	Mesh Meshes[];
};

struct BVHNode
{
	vec3 lmin; uint left;
	vec3 lmax; uint right;
	vec3 rmin; uint triCount;
	vec3 rmax; uint firstTri;
};

uniform uint NumBVHNodes = 0;
// Binding 8 is occupied by the materials. I'm just too lazy to refactor that...
layout (std430, binding = 4) readonly buffer BVHNodeBuffer
{
	BVHNode BVHNodes[];
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
	vec3 s = ray.origin - (v0.position + mesh.position);
	float u = inv_det * dot(s, ray_cross_e2);
	if (u < 0.0 || u > 1.0) return;
	vec3 s_cross_e1 = cross(s, edge1);
	float v = inv_det * dot(ray.dir, s_cross_e1);
	if (v < 0.0 || v + u > 1.0) return;
	float t = inv_det * dot(edge2, s_cross_e1);

	if (t > 0.0 && t < ray.hit.dst)
	{
		ray.hit.didHit = true;
		ray.hit.inside = det < 0.0;
		ray.hit.materialIndex = mesh.materialIndex;
		ray.hit.dst = t;
		ray.hit.hitPoint = ray.origin + ray.dir * t;
		ray.hit.barycentrics = vec2(u, v);
		float w = 1.0 - u - v;

		// Sorry for a bit of Jacco code here
		vec3 v0n = vec3(0.0); vec3 v0t = vec3(0.0); UnpackNormalTangent(v0.normalTanget, v0n, v0t);
		vec3 v1n = vec3(0.0); vec3 v1t = vec3(0.0); UnpackNormalTangent(v1.normalTanget, v1n, v1t);
		vec3 v2n = vec3(0.0); vec3 v2t = vec3(0.0); UnpackNormalTangent(v2.normalTanget, v2n, v2t);

		ray.hit.normal = normalize(u * v1n + v * v2n + w * v0n) * sign(det);
		ray.hit.tangent = normalize(u * v1t + v * v2t + w * v0t);
	}
}

void RayMesh(inout Ray ray, Mesh mesh)
{
	// Traverse the BVH of the mesh
	uint nodeOffset = mesh.firstBvhNode;
	uint triOffset = mesh.firstIndex;

	uint stack[128];
	int stackIndex = 0;
	stack[stackIndex++] = nodeOffset;

	while (stackIndex > 0)
	{
		BVHNode node = BVHNodes[stack[--stackIndex]];

		if (node.triCount > 0 /* Leaf node */)
		{
			for (int i = 0; i < node.triCount; i++)
			{
				uint indexBase = triOffset + 3 * (node.firstTri + i);
				RayTriangle(
					ray, mesh,
					Vertices[Indices[indexBase + 0]],
					Vertices[Indices[indexBase + 1]],
					Vertices[Indices[indexBase + 2]]
				);
			}
		}
		else
		{
			// Altered version of traverse_ailalaine (as found in https://github.com/jbikker/tinybvh/blob/main/traverse_bvh2.cl)
			const vec3 rD = ray.invDir;
			const vec3 rO = ray.origin * -rD;
			const vec3 t1a = fma( node.lmin + mesh.position, rD, rO ), t2a = fma( node.lmax + mesh.position, rD, rO );
			const vec3 t1b = fma( node.rmin + mesh.position, rD, rO ), t2b = fma( node.rmax + mesh.position, rD, rO );
			const vec3 minta = min( t1a, t2a ), maxta = max( t1a, t2a );
			const vec3 mintb = min( t1b, t2b ), maxtb = max( t1b, t2b );
			const float tmina = max( max( max( minta.x, minta.y ), minta.z ), 0 );
			const float tminb = max( max( max( mintb.x, mintb.y ), mintb.z ), 0 );
			const float tmaxa = min( min( min( maxta.x, maxta.y ), maxta.z ), ray.hit.dst );
			const float tmaxb = min( min( min( maxtb.x, maxtb.y ), maxtb.z ), ray.hit.dst );

			float dist1 = tmina > tmaxa ? 1e30 : tmina;
			float dist2 = tminb > tmaxb ? 1e30 : tminb;
			uint left = nodeOffset + node.left, right = nodeOffset + node.right;
			if (dist1 > dist2)
			{
				float h = dist1; dist1 = dist2; dist2 = h;
				uint t = left; left = right; right = t;
			}
			if (dist1 < 1e29)
				stack[stackIndex++] = left;
			if (dist2 < 1e29)
				stack[stackIndex++] = right;
		}
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
	for (uint i = 0; i < NumSpheres; ++i)
		RaySphere(ray, Spheres[i]);

	for (uint i = 0; i < NumMeshes; ++i)
		RayMesh(ray, Meshes[i]);
}

#endif // INTERSECTIONS_GLSL