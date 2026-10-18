#pragma once
#include <DirectXMath.h>
#include <string>
namespace dx = DirectX;

class Material
{
private:

	dx::XMFLOAT3 kd; // diffuse color
	dx::XMFLOAT3 ka; // ambient color
	dx::XMFLOAT3 ks; // specular color
	float ns; // specular exponent
	dx::XMFLOAT3 tf; // tf  Transmission Filter
	float ni; //Optical Density
	std::string map_Kd; // texture
	std::string bumb; // texture
	//"Tr" - Transparency

	bool hasTexture;
	bool hasABumpMap;

public:

	Material();

	void setAmbient(dx::XMFLOAT3 ambientColor);
	dx::XMFLOAT3 getAmbient() const;

	void setDiffuse(dx::XMFLOAT3 diffuseColor);
	dx::XMFLOAT3 getDiffuse() const;

	void setSpecular(dx::XMFLOAT3 specularColor);
	dx::XMFLOAT3 getSpecular() const;

	void setTf(dx::XMFLOAT3 tf);
	dx::XMFLOAT3 getTf() const;

	void setNs(float specularExponent);
	float getNs() const;

	void setNi(float ni);
	float getNi() const;

	void setHasTexture(bool hasTexture);
	bool getHasTexture() const;

	void setHasABumpMap(bool hasABumpMap);
	bool getHasABumpMap() const;

	void setTextureName(std::string textureName);
	std::wstring getTextureName() const;

	void setNormalTextureName(std::string textureName);
	std::wstring getNormalTextureName() const;
};

