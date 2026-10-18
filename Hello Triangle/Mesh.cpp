#include "Mesh.h"
Mesh::Mesh(ID3D11Device * devicePtr)
{
	this->devicePtr = devicePtr;
}

Mesh::Mesh(ID3D11Device * devicePtr, bool pickable)
{
	this->devicePtr = devicePtr;
	this->pickable = pickable;
}

dx::XMFLOAT3 Mesh::getTranslation() const
{
	return this->translation;
}

dx::XMFLOAT3 Mesh::getRotation() const
{
	return this->rotation;
}

dx::XMFLOAT3 Mesh::getScale() const
{
	return this->scale;
}

void Mesh::setTranslation(dx::XMFLOAT3 translation)
{
	this->translation = translation;
	translationMatrix = dx::XMMatrixTranslation(this->translation.x, this->translation.y, this->translation.z);
	setWorldMatrix();
}

void Mesh::setRotation(dx::XMFLOAT3 rotation)
{
	this->rotation = rotation;
	rotationMatrix = dx::XMMatrixRotationRollPitchYaw(this->rotation.x, this->rotation.y, this->rotation.z);
	setWorldMatrix();
}

void Mesh::setScale(dx::XMFLOAT3 scale)
{
	this->scale = scale;
	scaleMatrix = dx::XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
	setWorldMatrix();
}

void Mesh::setScale(float scale)
{
	this->scale.x = scale;
	this->scale.y = scale;
	this->scale.z = scale;

	scaleMatrix = dx::XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
	setWorldMatrix();
}

void Mesh::setWorldMatrix()
{
	this->worldMatrix = this->scaleMatrix * this->rotationMatrix * this->translationMatrix;
}

void Mesh::setWorldMatrix(dx::XMMATRIX matrix)
{
	this->worldMatrix = matrix;
}

dx::XMMATRIX Mesh::getTranslationMatrix() const
{
	return this->translationMatrix;
}

dx::XMMATRIX Mesh::getRotationMatrix() const
{
	return this->rotationMatrix;
}

dx::XMMATRIX Mesh::getScaleMatrix() const
{
	return this->scaleMatrix;
}

dx::XMMATRIX Mesh::getWorldMatrix() const
{
	return this->worldMatrix;
}

void Mesh::readModelFromeFile(std::string fileName)
{
	objPaser.readFromFile(fileName);
	vertexList = objPaser.getVertexList();
	this->vertexCount = (UINT)vertexList.size();
	createVertexBuffer();
	material = objPaser.getMaterial();
}

void Mesh::readTextureFromeFile(std::wstring fileName)
{
	HRESULT hr = dx::CreateWICTextureFromFile(devicePtr, fileName.data(), nullptr, &textureResourceViewPtr);
	assert(SUCCEEDED(hr));
	setHasTextureMap(TRUE);
}

void Mesh::readNormalTextureFromeFile(std::wstring fileName)
{
	HRESULT hr = dx::CreateWICTextureFromFile(devicePtr, fileName.data(), nullptr, &normalTextureResourceViewPtr);
	assert(SUCCEEDED(hr));
	setHasNormalMap(TRUE);
}

std::vector<Vertex> &Mesh::getVertexList()
{
	return this->vertexList;
}

BOOL Mesh::hasNormalMap() const
{
	return this->normalMap;
}

void Mesh::setHasNormalMap(BOOL normalMap)
{
	this->normalMap = normalMap;
}

BOOL Mesh::hasTextureMap() const
{
	return this->textureMap;
}

void Mesh::setHasTextureMap(BOOL textureMap)
{
	this->textureMap = textureMap;
}

Material &Mesh::getMaterial()
{
	return this->material;
}

void Mesh::setMaterial(Material material)
{
	this->material = material;
}

UINT Mesh::getVertexCount() const
{
	return this->vertexCount;
}

UINT &Mesh::getVertexStride()
{
	return this->vertexStride;
}

UINT &Mesh::getVertexOffset()
{
	return this->vertexOffset;
}

void Mesh::createVertexBuffer()
{
	unsigned int vertexDataArrayByteSize = sizeof(Vertex) * vertexList.size();

	D3D11_BUFFER_DESC vertexBuffDescr = {};
	vertexBuffDescr.ByteWidth = vertexDataArrayByteSize;
	vertexBuffDescr.Usage = D3D11_USAGE_DEFAULT; // helps the driver optimisation, based on how often the buffer values will change
	vertexBuffDescr.BindFlags = D3D11_BIND_VERTEX_BUFFER; //binding a buffer of type to the BindFlags
	vertexBuffDescr.CPUAccessFlags = 0;
	vertexBuffDescr.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA sr_Data = { 0 }; // points to our array of floats
	sr_Data.pSysMem = vertexList.data();

	HRESULT hr = devicePtr->CreateBuffer(&vertexBuffDescr, &sr_Data, &vertexBufferPtr);
	assert(SUCCEEDED(hr));//gives us a pointer to a vertex buffer.
}

void Mesh::release()
{
	if (vertexBufferPtr != nullptr)
	{
		vertexBufferPtr->Release();
		vertexBufferPtr = nullptr;
	}
	if (textureResourceViewPtr != nullptr)
	{
		textureResourceViewPtr->Release();
		textureResourceViewPtr = nullptr;

	}
	if (normalTextureResourceViewPtr != nullptr)
	{
		normalTextureResourceViewPtr->Release();
		normalTextureResourceViewPtr = nullptr;
	}
}

void Mesh::addVertex(float x, float y, float z, float u, float v, float nx, float ny, float nz)
{
	Vertex tempVertex(x,y,z,u,v,nx,ny,nz);
	vertexList.push_back(tempVertex);
	this->vertexCount = (UINT)vertexList.size();
}

void Mesh::setPickable(bool pickable)
{
	this->pickable = pickable;
}

bool Mesh::getPickable()
{
	return pickable;
}

float Mesh::getSpeed()
{
	return speed;
}

void Mesh::setSpeed(float speed)
{
	this->speed = speed;
}