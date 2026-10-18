/* vertex attributes go here to input to the vertex shader */


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

struct VS_INPUT
{
    float3 vPosition : POSITION;
    float2 vTexture : TEXTURE;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 textureVar : TEXTURE;
    float3 normal : NORMAL;
    float3 worldPos : WORLDPOSITION;
    float3 tangent : TANGENT;
};



VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT outPut;
    
    outPut = (VS_OUTPUT) 0;
    outPut.position = mul(float4(input.vPosition, 1.0f), WVP);
    outPut.textureVar.x = input.vTexture.x + animationSpeed.x;
    outPut.textureVar.y = input.vTexture.y + animationSpeed.y;
    outPut.normal = mul(float4(input.vNormal, 0.0f), world);
    outPut.worldPos = mul(float4(input.vPosition, 1.0f), world);
    outPut.tangent = mul(float4(input.vTangent, 1.0f), world);

    return outPut;
}

struct PS_OUTPUT
{
    float4 position : SV_Target0;
    float4 textureColor : SV_Target1;
    float4 normal : SV_Target2;
};

Texture2D tex : TEXTURE : register(t0);
Texture2D normalMapTex : TEXTURE : register(t1);
SamplerState samplerState : SAMPLER : register(s0);


float3 normalMapFunc(VS_OUTPUT input)
{
    float3 normalMap = normalMapTex.Sample(samplerState, input.textureVar);
        
    normalMap = (2.0f * normalMap - 1.0f);
        
    float3 tangent = normalize(input.tangent - dot(input.tangent, input.normal) * input.normal);
        
    float3 biTangent = cross(input.normal, tangent);
        
    float3 bumpNormal = (normalMap.x * tangent) + (normalMap.y * biTangent) + (normalMap.z * input.normal);
    
    return bumpNormal;
}

PS_OUTPUT PS_Main(VS_OUTPUT input) : SV_TARGET //Pixel shader
{
    PS_OUTPUT outPut;
    outPut.position = float4(input.worldPos, 1);
    outPut.textureColor = tex.Sample(samplerState, input.textureVar);

    input.normal = normalize(input.normal);
    if (hasANormalMap == true)
    {
        input.normal = normalize(normalMapFunc(input));
    }
    outPut.normal = float4(input.normal, 0);

    return outPut;
    
}





