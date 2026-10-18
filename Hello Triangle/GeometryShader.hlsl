cbuffer GS_CONSTANT_BUFFER : register(b0)
{
    float3 cameraPos;
};

struct GS_INPUT
{
    float4 position : SV_POSITION;
    float2 textureVar : TEXTURE;
    float3 normal : NORMAL;
    float3 worldPos : WORLDPOSITION;
    float3 tangent : TANGENT;
};

struct GS_OUTPUT
{    
    float4 position : SV_POSITION;
    float2 textureVar : TEXTURE;
    float3 normal : NORMAL;
    float3 worldPos : WORLDPOSITION;
    float3 tangent : TANGENT;
};

[maxvertexcount(3)]
void main(triangle GS_INPUT input[3], inout TriangleStream<GS_OUTPUT> output)
{
    float3 edge1 = input[1].worldPos - input[0].worldPos;
    float3 edge2 = input[2].worldPos - input[0].worldPos;
    float3 normal = cross(edge2, edge1);
    
    float3 cameraToTriangle = input[0].worldPos - cameraPos;
    cameraToTriangle = normalize(cameraToTriangle);
    
    if (dot(normal, cameraToTriangle) >= 0)
    {
        for (uint i = 0; i < 3; i++)
        {
            GS_OUTPUT element;
            element.position = input[i].position;
            element.textureVar = input[i].textureVar;
            element.normal = input[i].normal;
            element.worldPos = input[i].worldPos;
            element.tangent = input[i].tangent;
            output.Append(element);
        }
    }        
}