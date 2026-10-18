#include "Material.h"

Material::Material()
{
	ka = dx::XMFLOAT3(0, 0, 0);
	kd = dx::XMFLOAT3(0, 0, 0);
	ks = dx::XMFLOAT3(0, 0, 0);
	tf = dx::XMFLOAT3(0, 0, 0);
	ns = 0;
	ni = 0;
	map_Kd = "";
	bumb = "";
	hasTexture = false;
	hasABumpMap = false;

}

void Material::setAmbient(dx::XMFLOAT3 ambientColor)
{
	ka = ambientColor;
}
dx::XMFLOAT3 Material::getAmbient() const
{
	return this->ka;
}

void Material::setDiffuse(dx::XMFLOAT3 diffuseColor)
{
	kd = diffuseColor;
}

dx::XMFLOAT3 Material::getDiffuse() const
{
	return this->kd;
}

void Material::setSpecular(dx::XMFLOAT3 specularColor)
{
	ks = specularColor;
}

dx::XMFLOAT3 Material::getSpecular() const
{
	return this->ks;
}

void Material::setTf(dx::XMFLOAT3 tf)
{
	this->tf = tf;
}

dx::XMFLOAT3 Material::getTf() const
{
	return this->tf;
}

void Material::setNs(float specularExponent)
{
	ns = specularExponent;
}

float Material::getNs() const
{
	return this->ns;
}

void Material::setNi(float ni)
{
	this->ni = ni;
}

float Material::getNi() const
{
	return this->ni;
}

bool Material::getHasTexture() const
{
	return this->hasTexture;
}

void Material::setHasTexture(bool hasTexture)
{
	this->hasTexture = hasTexture;
}

bool Material::getHasABumpMap() const
{
	return this->hasABumpMap;
}

void Material::setHasABumpMap(bool hasABumpMap)
{
	this->hasABumpMap = hasABumpMap;
}

void Material::setTextureName(std::string textureName)
{
	this->map_Kd = textureName;
}

std::wstring Material::getTextureName() const
{
	std::wstring name;
	name.assign(map_Kd.begin(), map_Kd.end());
	return name;
}

void Material::setNormalTextureName(std::string textureName)
{
	this->bumb = textureName;
}

std::wstring Material::getNormalTextureName() const
{
	std::wstring name;
	name.assign(bumb.begin(), bumb.end());
	return name;
}
