#pragma once
#include <DirectXMath.h>
#include "Camera.h"

namespace dx = DirectX;
class Light
{
private:

	dx::XMFLOAT3 position;
	float padding;

	dx::XMFLOAT3 ambientColor;
	float padding1;

	dx::XMFLOAT4 diffuse;

	dx::XMFLOAT3 direction;
	float padding2;

	dx::XMFLOAT3 attenuation;
	float globalAmbient;

	Camera camera;

public:

	Light() {};

	Light(float x, float y, float z, float ambientR, float ambientG, float ambientB,
		float diffuseColorR, float diffuseColorG, float diffuseColorB, float diffuseColorA,
		float directionX, float directionY, float directionZ,
		float attenuationC, float attenuationL, float attenuationQ)
		: position(x, y, z), ambientColor(ambientR, ambientG, ambientB),
		diffuse(diffuseColorR, diffuseColorG, diffuseColorB, diffuseColorA),
		direction(directionX, directionY, directionZ),
		attenuation(attenuationC, attenuationL, attenuationQ) {};
		
	dx::XMFLOAT3 getPos() const;
	dx::XMFLOAT3 getAmb() const;
	dx::XMFLOAT4 getdiff() const;
	dx::XMFLOAT3 getDir() const;
	dx::XMFLOAT3 getAtt() const;
	float getGlobalAmb() const;

	void setPos(dx::XMFLOAT3 pos);
	void setAmb(dx::XMFLOAT3 amb);
	void setdiff(dx::XMFLOAT4 diff);
	void setDir(dx::XMFLOAT3 dir);
	void setAtt(dx::XMFLOAT3 att);
	void setGlobalAmb(float);
	
	Camera &getCamera();
};
