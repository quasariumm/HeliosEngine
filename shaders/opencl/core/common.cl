#ifndef COMMON_CL_
#define COMMON_CL_

__constant sampler_t texSampler = CLK_NORMALIZED_COORDS_TRUE |
                                  CLK_ADDRESS_CLAMP_TO_EDGE |
                                  CLK_FILTER_NEAREST;


struct GeometryContext
{
    struct Material* materials;
    struct Sphere* spheres;
    uint numSpheres;
    struct Mesh* meshes;
    struct BVHNode* bvhNodes;
    struct Vertex* vertices;
    uint* indices;
    uint numMeshes;
};

struct LightsContext
{
    struct DirectionalLight* directionalLights;
    uint numDirectionalLights;
    struct PointLight* pointLights;
    uint numPointLights;
    struct SimpleSpotLight* simpleSpotLights;
    uint numSimpleSpotLights;
};


float4 mat4_mul_vec4(float16 m, float4 v) {
    return (float4)(
        dot(m.s0123, v),
        dot(m.s4567, v),
        dot(m.s89AB, v),
        dot(m.sCDEF, v)
    );
}

float4 transform_point(float16 mat, float3 position) {
    float4 v = (float4)(position, 1.f);
    return (float4)(
        dot(mat.s0123, v),
        dot(mat.s4567, v),
        dot(mat.s89AB, v),
        dot(mat.sCDEF, v)
    );
}

float4 transform_vector(float16 mat, float3 vector) {
    float4 v = (float4)(vector, 0.f);
    return (float4)(
        dot(mat.s0123, v),
        dot(mat.s4567, v),
        dot(mat.s89AB, v),
        dot(mat.sCDEF, v)
    );
}

// Inverts a 4x4 matrix stored as float16 (row-major)
// Returns true if successful, false if matrix is singular
bool inverse_mat4(float16 m, float16* inv) {
    float16 temp;
    float det;
    
    // Calculate cofactors and determinant
    temp.s0 = m.s5 * m.sA * m.sF - m.s5 * m.sB * m.sE - m.s9 * m.s6 * m.sF + 
              m.s9 * m.s7 * m.sE + m.sD * m.s6 * m.sB - m.sD * m.s7 * m.sA;
    
    temp.s4 = -m.s4 * m.sA * m.sF + m.s4 * m.sB * m.sE + m.s8 * m.s6 * m.sF - 
               m.s8 * m.s7 * m.sE - m.sC * m.s6 * m.sB + m.sC * m.s7 * m.sA;
    
    temp.s8 = m.s4 * m.s9 * m.sF - m.s4 * m.sB * m.sD - m.s8 * m.s5 * m.sF + 
              m.s8 * m.s7 * m.sD + m.sC * m.s5 * m.sB - m.sC * m.s7 * m.s9;
    
    temp.sC = -m.s4 * m.s9 * m.sE + m.s4 * m.sA * m.sD + m.s8 * m.s5 * m.sE - 
               m.s8 * m.s6 * m.sD - m.sC * m.s5 * m.sA + m.sC * m.s6 * m.s9;
    
    temp.s1 = -m.s1 * m.sA * m.sF + m.s1 * m.sB * m.sE + m.s9 * m.s2 * m.sF - 
               m.s9 * m.s3 * m.sE - m.sD * m.s2 * m.sB + m.sD * m.s3 * m.sA;
    
    temp.s5 = m.s0 * m.sA * m.sF - m.s0 * m.sB * m.sE - m.s8 * m.s2 * m.sF + 
              m.s8 * m.s3 * m.sE + m.sC * m.s2 * m.sB - m.sC * m.s3 * m.sA;
    
    temp.s9 = -m.s0 * m.s9 * m.sF + m.s0 * m.sB * m.sD + m.s8 * m.s1 * m.sF - 
               m.s8 * m.s3 * m.sD - m.sC * m.s1 * m.sB + m.sC * m.s3 * m.s9;
    
    temp.sD = m.s0 * m.s9 * m.sE - m.s0 * m.sA * m.sD - m.s8 * m.s1 * m.sE + 
              m.s8 * m.s2 * m.sD + m.sC * m.s1 * m.sA - m.sC * m.s2 * m.s9;
    
    temp.s2 = m.s1 * m.s6 * m.sF - m.s1 * m.s7 * m.sE - m.s5 * m.s2 * m.sF + 
              m.s5 * m.s3 * m.sE + m.sD * m.s2 * m.s7 - m.sD * m.s3 * m.s6;
    
    temp.s6 = -m.s0 * m.s6 * m.sF + m.s0 * m.s7 * m.sE + m.s4 * m.s2 * m.sF - 
               m.s4 * m.s3 * m.sE - m.sC * m.s2 * m.s7 + m.sC * m.s3 * m.s6;
    
    temp.sA = m.s0 * m.s5 * m.sF - m.s0 * m.s7 * m.sD - m.s4 * m.s1 * m.sF + 
              m.s4 * m.s3 * m.sD + m.sC * m.s1 * m.s7 - m.sC * m.s3 * m.s5;
    
    temp.sE = -m.s0 * m.s5 * m.sE + m.s0 * m.s6 * m.sD + m.s4 * m.s1 * m.sE - 
               m.s4 * m.s2 * m.sD - m.sC * m.s1 * m.s6 + m.sC * m.s2 * m.s5;
    
    temp.s3 = -m.s1 * m.s6 * m.sB + m.s1 * m.s7 * m.sA + m.s5 * m.s2 * m.sB - 
               m.s5 * m.s3 * m.sA - m.s9 * m.s2 * m.s7 + m.s9 * m.s3 * m.s6;
    
    temp.s7 = m.s0 * m.s6 * m.sB - m.s0 * m.s7 * m.sA - m.s4 * m.s2 * m.sB + 
              m.s4 * m.s3 * m.sA + m.s8 * m.s2 * m.s7 - m.s8 * m.s3 * m.s6;
    
    temp.sB = -m.s0 * m.s5 * m.sB + m.s0 * m.s7 * m.s9 + m.s4 * m.s1 * m.sB - 
               m.s4 * m.s3 * m.s9 - m.s8 * m.s1 * m.s7 + m.s8 * m.s3 * m.s5;
    
    temp.sF = m.s0 * m.s5 * m.sA - m.s0 * m.s6 * m.s9 - m.s4 * m.s1 * m.sA + 
              m.s4 * m.s2 * m.s9 + m.s8 * m.s1 * m.s6 - m.s8 * m.s2 * m.s5;
    
    // Calculate determinant
    det = m.s0 * temp.s0 + m.s1 * temp.s4 + m.s2 * temp.s8 + m.s3 * temp.sC;
    
    // Check if matrix is singular
    if (fabs(det) < 1e-9f) {
        return false;
    }
    
    // Divide by determinant
    det = 1.0f / det;
    *inv = temp * det;
    
    return true;
}

#endif