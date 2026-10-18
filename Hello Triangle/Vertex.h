#pragma once
#include <DirectXMath.h>
namespace dx = DirectX;
class Vertex
{
private:
	dx::XMFLOAT3 pos = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	dx::XMFLOAT2 UV = dx::XMFLOAT2(0.0f, 0.0f);
	dx::XMFLOAT3 normal = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	dx::XMFLOAT3 tangent = dx::XMFLOAT3(0.0f,0.0f,0.0f);

public:
	Vertex() {};
	Vertex(float x, float y, float z, float u, float v,
		float normalX, float normalY, float normalZ/*,float tangentX, float tangentY, float tangentZ*/)
		: pos(x, y, z), UV(u, v), normal(normalX, normalY, normalZ)/*, tangent(tangentX, tangentY, tangentZ)*/{};

	dx::XMFLOAT3 getPos() const;
	void setPos(dx::XMFLOAT3 pos);

	dx::XMFLOAT2 getUV() const;
	void setUV(dx::XMFLOAT2 UV);

	dx::XMFLOAT3 getNormal() const;
	void setNormal(dx::XMFLOAT3 normal);

	dx::XMFLOAT3 getTangent() const;
	void setTangent(dx::XMFLOAT3 tangent);
};

