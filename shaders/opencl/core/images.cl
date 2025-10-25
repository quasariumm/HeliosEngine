struct Texture
{
    __global uchar* data;
    uint width;
    uint height;
    uchar channels;
};

struct HDRTexture
{
    __global float* data;
    uint width;
    uint height;
    uchar channels;
};

float3 SamplefHDRImage3(struct HDRTexture* tex, float2 uv)
{
    float2 pixel = uv * (float2)(tex->width, tex->height);
    uint2 topLeft = convert_uint2_rtn(pixel);

    float2 offset = pixel - convert_float2(topLeft);

    uint o00 = topLeft.y * tex->width + topLeft.x;
    float3 p00 = (float3)(tex->data[o00], tex->data[o00 + 1], tex->data[o00 + 2]);
    uint o01 = topLeft.y * tex->width + topLeft.x + 1;
    float3 p01 = (float3)(tex->data[o01], tex->data[o01 + 1], tex->data[o01 + 2]);
    uint o10 = (topLeft.y + 1) * tex->width + topLeft.x;
    float3 p10 = (float3)(tex->data[o10], tex->data[o10 + 1], tex->data[o10 + 2]);
    uint o11 = (topLeft.y + 1) * tex->width + topLeft.x + 1;
    float3 p11 = (float3)(tex->data[o11], tex->data[o11 + 1], tex->data[o11 + 2]);

    return mix(mix(p00, p01, offset.x), mix(p10, p11, offset.x), offset.y);
}

float4 SamplefHDRImage4(struct HDRTexture* tex, float2 uv)
{
    float2 pixel = uv * (float2)(tex->width, tex->height);
    uint2 topLeft = convert_uint2_rtn(pixel);

    float2 offset = pixel - convert_float2(topLeft);

    uint o00 = topLeft.y * tex->width + topLeft.x;
    float4 p00 = (float4)(tex->data[o00], tex->data[o00 + 1], tex->data[o00 + 2], tex->data[o00 + 3]);
    uint o01 = topLeft.y * tex->width + topLeft.x + 1;
    float4 p01 = (float4)(tex->data[o01], tex->data[o01 + 1], tex->data[o01 + 2], tex->data[o01 + 3]);
    uint o10 = (topLeft.y + 1) * tex->width + topLeft.x;
    float4 p10 = (float4)(tex->data[o10], tex->data[o10 + 1], tex->data[o10 + 2], tex->data[o10 + 3]);
    uint o11 = (topLeft.y + 1) * tex->width + topLeft.x + 1;
    float4 p11 = (float4)(tex->data[o11], tex->data[o11 + 1], tex->data[o11 + 2], tex->data[o11 + 3]);

    return mix(mix(p00, p01, offset.x), mix(p10, p11, offset.x), offset.y);
}

float4 SampleiHDRImage4(struct HDRTexture* tex, int2 pixel)
{
    uint offset = pixel.y * tex->width + pixel.x;
    float4 out;
    out.r = tex->data[offset + 0];
    out.g = tex->data[offset + 1];
    out.b = tex->data[offset + 2];
    out.a = tex->data[offset + 3];
    return out;
}

void WriteHDRImage4(struct HDRTexture* tex, int2 pixel, float4 value)
{
    uint offset = pixel.y * tex->width + pixel.x;
    tex->data[offset + 0] = value.r;
    tex->data[offset + 1] = value.g;
    tex->data[offset + 2] = value.b;
    tex->data[offset + 3] = value.a;
}