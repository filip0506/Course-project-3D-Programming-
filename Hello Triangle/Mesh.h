#pragma once
#include "Vertex.h"
#include <DirectXMath.h>
#include <vector>
#include <d3d11.h> // D3D 11
#include <dxgi.h> // graphics devices and swap chain
#include "OBJParser.h"
#include <WICTextureLoader.h>

class Mesh
{
private:
	
	ID3D11Device *devicePtr = nullptr;

	UINT vertexStride = sizeof(Vertex); // spacing of the elements in the array
	UINT vertexOffset = 0;
	UINT vertexCount = 0;
	BOOL normalMap = FALSE;
	BOOL textureMap = FALSE;

	dx::XMFLOAT3 translation; // translation;
	dx::XMFLOAT3 rotation;
	dx::XMFLOAT3 scale;

	dx::XMMATRIX translationMatrix = dx::XMMatrixIdentity(); // translation;
	dx::XMMATRIX rotationMatrix = dx::XMMatrixIdentity();
	dx::XMMATRIX scaleMatrix = dx::XMMatrixIdentity();

	dx::XMMATRIX worldMatrix = dx::XMMatrixIdentity();

	std::vector<Vertex> vertexList;
	OBJParser objPaser;
	Material material;

	bool pickable = false;
	float speed = 0.01f;

public:

	Mesh(ID3D11Device *devicePtr);
	Mesh(ID3D11Device *devicePtr, bool pickable);

	ID3D11Buffer *vertexBufferPtr;
	ID3D11ShaderResourceView *textureResourceViewPtr; //A shader-resource-view interface specifies the subresources a shader can access during rendering
	ID3D11ShaderResourceView *normalTextureResourceViewPtr;

	dx::XMFLOAT3 getTranslation() const;
	dx::XMFLOAT3 getRotation() const;
	dx::XMFLOAT3 getScale() const;

	void setTranslation(dx::XMFLOAT3 translation);
	void setRotation(dx::XMFLOAT3 rotation);
	void setScale(dx::XMFLOAT3 scale);
	void setScale(float scale);

	void setWorldMatrix();
	void setWorldMatrix(dx::XMMATRIX matrix);

	dx::XMMATRIX getTranslationMatrix() const;
	dx::XMMATRIX getRotationMatrix() const;
	dx::XMMATRIX getScaleMatrix() const;
	dx::XMMATRIX getWorldMatrix() const;

	void readModelFromeFile(std::string fileName);
	void readTextureFromeFile(std::wstring fileName);
	void readNormalTextureFromeFile(std::wstring fileName);

	std::vector<Vertex> &getVertexList();

	BOOL hasNormalMap() const;
	void setHasNormalMap(BOOL normalMap);

	BOOL hasTextureMap() const;
	void setHasTextureMap(BOOL textureMap);

	Material &getMaterial();
	void setMaterial(Material material);

	UINT getVertexCount() const;
	UINT &getVertexStride();
	UINT &getVertexOffset();

	void createVertexBuffer();
	void release();
	void addVertex(float x, float y, float z, float u, float v, float nx, float ny, float nz);

	void setPickable(bool pickable);
	bool getPickable();

	float getSpeed();
	void setSpeed(float speed);
};