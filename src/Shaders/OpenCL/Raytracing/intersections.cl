#ifndef INTERSECTIONS_CL_
#define INTERSECTIONS_CL_

#include "Core/common.cl"
#include "Raytracing/materials.cl"

/*
	Holds the primitve structs and the ray-primitive intersection logic
*/

struct Sphere
{
	float3 center;
	float radius;
	int materialIndex;
};

bool RaySphere(struct Ray* ray, struct Sphere* sphere)
{
	// Thanks for the code, Sebastian Lague
	float3 oc = ray->origin - sphere->center;
	float b = dot(oc, ray->dir);
	float d = b * b - (dot(oc, oc) - sphere->radius * sphere->radius);

	if (d <= 0.f)
		return false;

	float sqrt_d = sqrt(d);
	float t1 = -b - sqrt_d;
	float t2 = -b + sqrt_d;

	if (t1 >= 0.f && t1 < ray->hit.dst)
	{
		ray->hit.hit = true;
		ray->hit.inside = b < 0.f;
		ray->hit.materialIndex = sphere->materialIndex;
		ray->hit.dst = t1;
		ray->hit.hitPoint = ray->origin + ray->dir * t1;
		ray->hit.normal = normalize(ray->hit.hitPoint - sphere->center);

		// Calculate tangent vector
		float3 arbitraryDirection = (float3)(1.f, 0.f, 0.f); // Choose an arbitrary direction
		if (dot(ray->hit.normal, arbitraryDirection) > 0.99f) // Check if it's too close to the normal
		arbitraryDirection = (float3)(0.f, 1.f, 0.f); // Choose another direction

		ray->hit.tangent = normalize(cross(ray->hit.normal, arbitraryDirection));
		return true;
	}

	if (t2 >= 0.f && t2 < ray->hit.dst && t1 <= 0.f)
	{
		ray->hit.hit = true;
		ray->hit.inside = b < 0.f;
		ray->hit.materialIndex = sphere->materialIndex;
		ray->hit.dst = t2;
		ray->hit.hitPoint = ray->origin + ray->dir * t2;
		ray->hit.normal = normalize(ray->hit.hitPoint - sphere->center) * sign(b);

		// Calculate tangent vector
		float3 arbitraryDirection = (float3)(1.f, 0.f, 0.f); // Choose an arbitrary direction
		if (dot(ray->hit.normal, arbitraryDirection) > 0.99f) // Check if it's too close to the normal
		arbitraryDirection = (float3)(0.f, 1.f, 0.f); // Choose another direction

		ray->hit.tangent = normalize(cross(ray->hit.normal, arbitraryDirection));
		return true;
	}
    return false;
}

/*
	Triangles
*/

struct Vertex
{
	float3 position;
	// The normal is in the first 16 bits of an axis, tangent in the other 16 bits
	// This needs to be a uint because of parameter reasons
	float texCoordX;
	half3 normal;
    half3 tangent;
	// Bitangent is cross(normal, tangent)
	float texCoordY;
};

struct Mesh
{
	float3 position;
	int materialIndex;
	uint indexCount;
	uint firstIndex;
	uint bvhNodeCount;
	uint firstBvhNode;
};

struct BVHNode
{
	float3 lmin; uint left;
	float3 lmax; uint right;
	float3 rmin; uint triCount;
	float3 rmax; uint firstTri;
};

bool RayTriangle(struct Ray* ray, struct Mesh* mesh, struct Vertex* v0, struct Vertex* v1, struct Vertex* v2)
{
	// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm#C++_implementation
	float3 edge1 = v1->position - v0->position;
	float3 edge2 = v2->position - v0->position;
	float3 ray_cross_e2 = cross(ray->dir, edge2);
	float det = dot(edge1, ray_cross_e2);

	if (fabs(det) < 0.00001f) return false;

	float inv_det = native_recip(det);
	float3 s = ray->origin - (v0->position + mesh->position);
	float u = inv_det * dot(s, ray_cross_e2);
	if (u < 0.f || u > 1.f) return false;
	float3 s_cross_e1 = cross(s, edge1);
	float v = inv_det * dot(ray->dir, s_cross_e1);
	if (v < 0.f || v + u > 1.f) return false;
	float t = inv_det * dot(edge2, s_cross_e1);

	if (t > 0.f && t < ray->hit.dst)
	{
		ray->hit.hit = true;
		ray->hit.inside = det < 0.f;
		ray->hit.materialIndex = mesh->materialIndex;
		ray->hit.dst = t;
		ray->hit.hitPoint = ray->origin + ray->dir * t;
		ray->hit.barycentric = (float2)(u, v);
		float w = 1.0 - u - v;

		ray->hit.normal = normalize(u * vload_half3(0, (half*)&v1->normal) + v * vload_half3(0, (half*)&v2->normal) + w * vload_half3(0, (half*)&v0->normal)) * sign(det);
		ray->hit.tangent = normalize(u * vload_half3(0, (half*)&v1->tangent) + v * vload_half3(0, (half*)&v2->tangent) + w * vload_half3(0, (half*)&v0->tangent)) * sign(det);
        return true;
	}

    return false;
}


bool RayMesh(struct Ray* ray, struct Mesh* mesh, struct BVHNode* bvhNodes, struct Vertex* vertices, uint* indices)
{
	// Traverse the BVH of the mesh
	uint nodeOffset = mesh->firstBvhNode;
	uint triOffset = mesh->firstIndex;

	uint stack[128];
	int stackIndex = 0;
	stack[stackIndex++] = nodeOffset;

    bool found = false;

	while (stackIndex > 0)
	{
		struct BVHNode* node = &bvhNodes[stack[--stackIndex]];

		if (node->triCount > 0 /* Leaf node */)
		{
			for (int i = 0; i < node->triCount; i++)
			{
				uint indexBase = triOffset + 3 * (node->firstTri + i);
				bool res = RayTriangle(
					ray, mesh,
					&vertices[indices[indexBase + 0]],
					&vertices[indices[indexBase + 1]],
					&vertices[indices[indexBase + 2]]
				);
                if (res) found = true;
			}
		}
		else
		{
			// Altered version of traverse_ailalaine (as found in https://github.com/jbikker/tinybvh/blob/main/traverse_bvh2.cl)
			const float3 rD = ray->invDir;
			const float3 rO = ray->origin * -rD;
			const float3 t1a = fma( node->lmin + mesh->position, rD, rO ), t2a = fma( node->lmax + mesh->position, rD, rO );
			const float3 t1b = fma( node->rmin + mesh->position, rD, rO ), t2b = fma( node->rmax + mesh->position, rD, rO );
			const float3 minta = min( t1a, t2a ), maxta = max( t1a, t2a );
			const float3 mintb = min( t1b, t2b ), maxtb = max( t1b, t2b );
			const float tmina = max( max( max( minta.x, minta.y ), minta.z ), 0.f );
			const float tminb = max( max( max( mintb.x, mintb.y ), mintb.z ), 0.f );
			const float tmaxa = min( min( min( maxta.x, maxta.y ), maxta.z ), ray->hit.dst );
			const float tmaxb = min( min( min( maxtb.x, maxtb.y ), maxtb.z ), ray->hit.dst );

			float dist1 = tmina > tmaxa ? HUGE_VALF : tmina;
			float dist2 = tminb > tmaxb ? HUGE_VALF : tminb;
			uint left = nodeOffset + node->left, right = nodeOffset + node->right;
			if (dist1 > dist2)
			{
				float h = dist1; dist1 = dist2; dist2 = h;
				uint t = left; left = right; right = t;
			}
			if (dist1 < 1e29f)
				stack[stackIndex++] = left;
			if (dist2 < 1e29f)
				stack[stackIndex++] = right;
		}
	}

    return found;
}

/*
	Scene collision
*/

bool RayCollision(
    struct Ray* ray,
    struct GeometryContext* ctx
)
{
	ray->hit.hit = false;
	ray->hit.dst = 1e30;
	for (int i = 0; i < ctx->numSpheres; ++i)
		RaySphere(ray, &ctx->spheres[i]);

	for (uint i = 0; i < ctx->numMeshes; ++i)
		RayMesh(ray, &ctx->meshes[i], ctx->bvhNodes, ctx->vertices, ctx->indices);

    return ray->hit.hit;
}

#endif