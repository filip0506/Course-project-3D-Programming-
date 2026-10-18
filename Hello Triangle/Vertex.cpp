#include "Vertex.h"

dx::XMFLOAT3 Vertex::getPos() const
{
	return this->pos;
}

void Vertex::setPos(dx::XMFLOAT3 pos)
{
	this->pos = pos;
}

dx::XMFLOAT2 Vertex::getUV() const
{
	return this->UV;
}

void Vertex::setUV(dx::XMFLOAT2 UV)
{
	this->UV = UV;
}

dx::XMFLOAT3 Vertex::getNormal() const
{
	return this->normal;
}

void Vertex::setNormal(dx::XMFLOAT3 normal)
{
	this->normal = normal;
}

dx::XMFLOAT3 Vertex::getTangent() const
{
	return this->tangent;
}

void Vertex::setTangent(dx::XMFLOAT3 tangent)
{
	this->tangent = tangent;
}
