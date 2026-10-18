#pragma once
#include "MyIncludes.h"
#include "Vertex.h"
#include <vector>
#include <WICTextureLoader.h>
#include <iostream>
#include "Camera.h"
#include "Light.h"
#include <math.h>
#include "Material.h"

namespace dx = DirectX;

class Terrain
{
private:

	std::vector<Vertex> vertexList;
	std::vector<UINT> indicesList;

	std::vector<dx::XMFLOAT3> heightMap; // stores height mapValues;
	std::vector<float> actualHeight;

	UINT terrainWidth = 0;
	UINT terrainHeight = 0;

	UINT vertexStride = sizeof(Vertex);
	UINT vertexOffset = 0;
	UINT vertexCount = 0;
	UINT indicesCount = 0;
	UINT numberOfFaces = 0;
	UINT numberOfVertices = 0;

	ID3D11Device *devicePtr;

	float gridSize;

	dx::XMFLOAT3 translation;
	dx::XMFLOAT3 rotation;
	dx::XMFLOAT3 scale;

	dx::XMMATRIX translationMatrix = dx::XMMatrixIdentity();
	dx::XMMATRIX rotationMatrix = dx::XMMatrixIdentity();
	dx::XMMATRIX scaleMatrix = dx::XMMatrixIdentity();

	dx::XMMATRIX worldMatrix = dx::XMMatrixIdentity();

	ID3D11ShaderResourceView *textureResourceViewPtr;
	ID3D11ShaderResourceView *normalTextureResourceViewPtr;

	ID3D11Buffer *vertexBuffer;
	ID3D11Buffer *indexBuffer;

	Material material;

	HRESULT hr;

public:

	Terrain();
	Terrain(ID3D11Device *devicePtr);

	void calculateAverageNormal();

	void generateTerrainNoMap(UINT width, UINT height);
	void generateTerrainMap(std::string filePath, float heightFactor);
	void generatePlane(UINT width, UINT height);

	void createVertexAndIndexBuffers();
	bool loadHeightMapSTB(std::string filename,float heightFactor);

	void release();

	void drawDeferred(Camera &camera, Light & light, ID3D11DeviceContext *& deviceContextPtr,  ID3D11SamplerState *& samplerStatePtr, ID3D11Buffer *& constantBufferPerObjectPtr, ConstantBuffer & constantBuffer, ID3D11Buffer *& constantBufferPtrMaterial, MaterialBuffer &material, dx::XMFLOAT4 speed);
	void drawDeferred(Camera &camera, Light & light, ID3D11DeviceContext *& deviceContextPtr, ID3D11GeometryShader *& geometryShaderPtr,ID3D11SamplerState *& samplerStatePtr, ID3D11Buffer *& constantBufferPerObjectPtr, ConstantBuffer & constantBuffer, ID3D11Buffer *& constantBufferPtrMaterial, MaterialBuffer &material, ID3D11Buffer *&constantBufferPtrBackFaceCulling, GeometryShaderBuffer &geometryShaderBuffer);
	void drawShadow(Camera &camera, Light & light, ID3D11DeviceContext *& deviceContextPtr, ID3D11VertexShader *&vertexShaderPtr, ID3D11Buffer *&constantBufferPerObjectPtr, ConstantBuffer &constantBuffer);

	void readTextureFromFile(std::wstring fileName);
	void readNormalTextureFromFile(std::wstring fileName);

	std::vector<Vertex> &getVertexList();
	std::vector<UINT> &getIndicesList();

	dx::XMFLOAT3 getTranslation() const;
	dx::XMFLOAT3 getRotation() const;
	dx::XMFLOAT3 getScale() const;

	void setTranslation(dx::XMFLOAT3 translation);
	void setRotation(dx::XMFLOAT3 rotation);
	void setScale(dx::XMFLOAT3 scale);

	void setWorldMatrix();
	void setWorldMatrix(dx::XMMATRIX matrix);

	dx::XMMATRIX getTranslationMatrix() const;
	dx::XMMATRIX getRotationMatrix() const;
	dx::XMMATRIX getScaleMatrix() const;
	dx::XMMATRIX getWorldMatrix() const;

	UINT getVertexCount() const;
	UINT &getVertexStride();
	UINT &getVertexOffset();
	UINT getIndicesCount() const;

	Material &getMaterial();
	void setMaterial(Material material);

	float getTerrainHeight(float x, float z);
};

