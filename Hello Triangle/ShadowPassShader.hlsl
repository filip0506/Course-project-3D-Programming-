cbuffer cBuf : register(b0) // per object
{
    matrix WVP;
    matrix world;
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
    float3 cameraPos;
    int hasANormalMap;
    float4 animationSpeed;

};

struct Light
{
    float3 position;
    float pad;
    float3 ambientColor;
    float pad1;
    float4 diffuse;
    float3 direction;
    float pad2;
    float3 attenuation;
    float globalAmbient;
};

cbuffer light : register(b2)
{
    Light light;
};

struct VS_INPUT
{
    float3 vPosition : POSITION;
    float2 vTexture : TEXTURE;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
};

float4 VS_Main(VS_INPUT input) : SV_Position //Vertex shader
{
    float4 lightPosition = mul(float4(input.vPosition, 1), WVP);
    return lightPosition;
}








