#include "Light.h"

dx::XMFLOAT3 Light::getPos() const
{
	return this->position;
}

dx::XMFLOAT3 Light::getAmb() const
{
	return this->ambientColor;
}

dx::XMFLOAT4 Light::getdiff() const
{
	return this->diffuse;
}

dx::XMFLOAT3 Light::getDir() const
{
	return this->direction;
}

dx::XMFLOAT3 Light::getAtt() const
{
	return this->attenuation;
}

float Light::getGlobalAmb() const
{
	return this->globalAmbient;
}

void Light::setPos(dx::XMFLOAT3 pos)
{
	position = pos;
	camera.setPosition(dx::XMFLOAT4(pos.x, pos.y, pos.z, 0));
}

void Light::setAmb(dx::XMFLOAT3 amb)
{
	ambientColor = amb;
}

void Light::setdiff(dx::XMFLOAT4 diff)
{
	diffuse = diff;
}

void Light::setDir(dx::XMFLOAT3 dir)
{
	direction = dir;
}

void Light::setAtt(dx::XMFLOAT3 att)
{
	attenuation = att;
}

void Light::setGlobalAmb(float globalAmb)
{
	this->globalAmbient = globalAmb;
}

Camera & Light::getCamera()
{
	return this->camera;
}



