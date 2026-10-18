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

struct MaterialCbuff
{
    float3 kd; // ambient color
    float3 ka; // diffuse color
    float3 ks; // specular color
    float ns; // specular exponent
};
cbuffer material : register(b1)
{
    MaterialCbuff material;
}

struct LightCbuff
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
    LightCbuff light;
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
    float4 posLightH : LIGHTPOS;
};

VS_OUTPUT VS_Main(VS_INPUT input) //Vertex shader
{
    VS_OUTPUT outPut;
    
    outPut = (VS_OUTPUT) 0;
    outPut.position = float4(input.vPosition, 1);
    outPut.textureVar = input.vTexture;

    outPut.normal = float4(input.vNormal, 0.0f);
    outPut.worldPos = mul(float4(input.vPosition, 1.0f), world);
    outPut.tangent = float4(input.vTangent, 1.0f);

    return outPut;
}

Texture2D pos : TEXTURE : register(t0);
Texture2D tex : TEXTURE : register(t1);
Texture2D norm : TEXTURE : register(t2);
Texture2D shadowMapTex : TEXTURE : register(t3);

SamplerState samplerState : SAMPLER : register(s0);
SamplerState samplerStateClamp : SAMPLER : register(s1);

float4 diffuseFunc(LightCbuff light, float3 L, float3 N, float3 kd)
{
    float LdotN = max(0, dot(L, N)); //intensity
    float4 diffuse = LdotN * light.diffuse * float4(kd, 1);
    
    return diffuse;
}

float3 specularFunc(LightCbuff light, float3 position, float3 L, float3 N, float3 ks, float ns)
{
    float3 V = normalize(cameraPos - position);
    float3 R = normalize(reflect(-L, N));

    float RdotV = max(0, dot(V, R));
    RdotV = pow(RdotV, ns);
    
    float3 spec = light.ambientColor * RdotV * ks;
    return spec;

}

float4 ambientFunc(LightCbuff light, float3 ka)
{
    float4 ambientLight = float4((light.ambientColor * light.globalAmbient) * ka, 1);
    return ambientLight;
}



float4 PS_Main(VS_OUTPUT input) : SV_TARGET //Pixel shader
{
    //Gbuffers    
    float4 positionTexture = pos.Load(float3(input.position.xy, 0), 0);
    float4 colorTexture = tex.Load(float3(input.position.xy, 0), 0);
    float4 normalTexture = norm.Load(float3(input.position.xy, 0), 0);
    float4 shadowTexture = shadowMapTex.Sample(samplerState, input.textureVar);
    
    input.posLightH = float4(positionTexture.xyz, 1);
    input.posLightH = mul(input.posLightH, lightViewMatrix);
    input.posLightH = mul(input.posLightH, lightProjectionMatrix);
    
    float SHADOW_EPSILION = 0.0141f;

    int width = 0;
    int height = 0;
    shadowMapTex.GetDimensions(width, height);
    
    input.posLightH.xy /= input.posLightH.w; // NDC
    float2 smTex = float2(0.5f * input.posLightH.x + 0.5f, -0.5f * input.posLightH.y + 0.5f);
    float depth = input.posLightH.z / input.posLightH.w;
        
    float dx = 1.0f / width;
    float s0 = (shadowMapTex.Sample(samplerStateClamp, smTex).r + SHADOW_EPSILION < depth) ? 0.0f : 1.0f;
    float s1 = (shadowMapTex.Sample(samplerStateClamp, smTex + float2(dx, 0.0f)).r + SHADOW_EPSILION < depth) ? 0.0f : 1.0f;
    float s2 = (shadowMapTex.Sample(samplerStateClamp, smTex + float2(0.0f, dx)).r + SHADOW_EPSILION < depth) ? 0.0f : 1.0f;
    float s3 = (shadowMapTex.Sample(samplerStateClamp, smTex + float2(dx, dx)).r + SHADOW_EPSILION < depth) ? 0.0f : 1.0f;
       
    float2 texelPos = smTex * width;
    float2 lerps = frac(texelPos);
    float shadowCoeff = lerp(lerp(s0, s1, lerps.x), lerp(s2, s3, lerps.x), lerps.y);


    float4 finalColor = float4(0, 0, 0, 0);
 
    float3 N = normalTexture;
    float3 L = normalize(-light.direction); //vector from light to point  

    float3 ambient = ambientFunc(light, material.ka);
    float4 diffuse = diffuseFunc(light, L, N, material.kd);
    float3 specular = specularFunc(light, positionTexture.xyz, L, N, material.ks, material.ns);
    
    float distance = length(L);
    finalColor = colorTexture * (float4(ambient, 1) + diffuse);
    finalColor = saturate(finalColor + float4(specular, 1));

    finalColor = float4(finalColor.rgb, 1) * shadowCoeff;

    return finalColor;
        
}

