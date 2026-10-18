#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Vertex.h"
#include "Material.h"
#include <DirectXMath.h>
#include <vector>
namespace dx = DirectX;

class OBJParser
{
private:

	std::vector<Vertex> vertexList;
	std::vector<unsigned int> indicesList;
	std::vector<dx::XMFLOAT3> v; //Vertex Position
	std::vector<dx::XMFLOAT2> vt; //Vertex Texture aka uv
	std::vector<dx::XMFLOAT3> vn; // Vertex Normal
	std::vector<dx::XMINT3> f; // Faces
	std::vector<dx::XMFLOAT3> tangentVec; // Vertex tangent

	Material material;
	dx::XMFLOAT3 kd; // diffuse color
	dx::XMFLOAT3 ka; // ambient color
	dx::XMFLOAT3 ks; // specular color
	float ns; // specular exponent
	dx::XMFLOAT3 tf; // tf
	float ni;
	std::string map_Kd; // texture
	std::string bump;

	bool hasATexture;
	bool hasABumpMap;

public:

	void readFromFile(std::string fileName);
	void readMtlFromFile(std::string fileName);
	void printObjInformation();
	void writeOutTestInformation(unsigned int vertexCount, unsigned int uvCount, unsigned int normalCount);
	
	std::vector<Vertex> &getVertexList();
	Material &getMaterial();
	std::wstring getTextureName() const;
	std::wstring getBumpName() const;

	bool returnHasATexture() const;
	bool returnHasABumpMap() const;

};

