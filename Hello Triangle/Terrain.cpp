#include "Terrain.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Terrain::Terrain()
{
}

Terrain::Terrain(ID3D11Device * devicePtr)
{
	this->devicePtr = devicePtr;
}

void Terrain::calculateAverageNormal()
{
	std::vector<dx::XMFLOAT3> tempNormal;
	tempNormal.resize(numberOfFaces);

	dx::XMFLOAT3 unnormalized = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);

	dx::XMVECTOR edge1 = dx::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	dx::XMVECTOR edge2 = dx::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	int vertex1;
	int vertex2;
	int vertex3;

	for (UINT i = 0; i < numberOfFaces; i++)
	{
		vertex1 = indicesList[(i * 3)];
		vertex2 = indicesList[(i * 3) + 1];
		vertex3 = indicesList[(i * 3) + 2];
		
		edge1 = dx::XMVectorSet(vertexList[vertex2].getPos().x - vertexList[vertex1].getPos().x, vertexList[vertex2].getPos().y - vertexList[vertex1].getPos().y, vertexList[vertex2].getPos().z - vertexList[vertex1].getPos().z, 0); // edge1
		edge2 = dx::XMVectorSet(vertexList[vertex3].getPos().x - vertexList[vertex1].getPos().x, vertexList[vertex3].getPos().y - vertexList[vertex1].getPos().y, vertexList[vertex3].getPos().z - vertexList[vertex1].getPos().z, 0); // edge2
		
		//Cross multiply the two edge vectors to get the un-normalized face normal
		dx::XMStoreFloat3(&unnormalized, dx::XMVector3Cross(edge1, edge2));
		tempNormal[i] = unnormalized;
	}

	//Compute vertex normals (normal Averaging)
	dx::XMVECTOR normalSum = dx::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	int facesUsing = 0;
	float tX;
	float tY;
	float tZ;

	std::cout << "NumVertices " << numberOfVertices << std::endl;
	std::cout << "NumFaces " << numberOfFaces << std::endl;

	//Go through each vertex
	for (UINT i = 0; i < numberOfVertices; ++i)
	{
		//Check which triangles use this vertex
		for (UINT j = 0; j < numberOfFaces; ++j)
		{
			if (indicesList[j * 3] == i || indicesList[(j * 3) + 1] == i || indicesList[(j * 3) + 2] == i)
			{
				tX = dx::XMVectorGetX(normalSum) + tempNormal[j].x;
				tY = dx::XMVectorGetY(normalSum) + tempNormal[j].y;
				tZ = dx::XMVectorGetZ(normalSum) + tempNormal[j].z;

				normalSum = dx::XMVectorSet(tX, tY, tZ, 0.0f);    //If a face is using the vertex, add the unormalized face normal to the normalSum
				facesUsing++;
			}
		}


		//Get the actual normal by dividing the normalSum by the number of faces sharing the vertex
		dx::XMVectorDivide(normalSum, dx::XMVectorSet((float)facesUsing, (float)facesUsing, (float)facesUsing, 0.0f));

		//Normalize the normalSum vector
		normalSum = dx::XMVector3Normalize(normalSum);

		//Store the normal in our current vertex
		vertexList[i].setNormal(dx::XMFLOAT3(dx::XMVectorGetX(normalSum), dx::XMVectorGetY(normalSum), dx::XMVectorGetZ(normalSum)));
		
		//Clear normalSum and facesUsing for next vertex
		normalSum = dx::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		facesUsing = 0;
	}
}

void Terrain::generateTerrainNoMap(UINT width, UINT height)
{
	numberOfFaces = 0;
	numberOfVertices = 0;
	UINT currentIndex = 0;
	
	numberOfVertices = width * height;
	numberOfFaces = (width - 1) * (height - 1) * 2;

	vertexList.resize(numberOfVertices);
	vertexCount = (UINT)vertexList.size();

	indicesList.resize(numberOfFaces * 3);
	indicesCount = (UINT)indicesList.size();

	gridSize = 1;

	for (UINT i = 0; i < height; i++)
	{
		for (UINT x = 0; x < width; x++)
		{
			currentIndex = (i * height) + x;

			vertexList[currentIndex].setPos(dx::XMFLOAT3((float)i, 0, (float)x));
			vertexList[currentIndex].setNormal(dx::XMFLOAT3(0, 1, 0));// or up
			vertexList[currentIndex].setUV(dx::XMFLOAT2((float)x, (float)i));
		}
	}

	int k = 0;

	for (DWORD i = 0; i < width - 1; i++)
	{
		for (DWORD x = 0; x < height - 1; x++)
		{
			indicesList[k] = i * height + x;
			indicesList[k + 1] = i * height + x + 1;
			indicesList[k + 2] = (i + 1) * height + x;
			indicesList[k + 3] = (i + 1)  * height + x;
			indicesList[k + 4] = i * height + x + 1;
			indicesList[k + 5] = (i + 1) * height + x + 1;
			k = k + 6;
		}
	}
}

void Terrain::generateTerrainMap(std::string filePath,float heightFactor)
{
	loadHeightMapSTB(filePath, heightFactor);

	UINT m = terrainHeight;
	UINT n = terrainWidth;

	numberOfVertices = m * n;
	numberOfFaces = (m - 1) * (n - 1) * 2;

	vertexList.resize(numberOfVertices);
	vertexCount = (UINT)vertexList.size();

	indicesList.resize(numberOfFaces * 3);
	indicesCount = (UINT)indicesList.size();

	actualHeight.resize(numberOfVertices);

	float halfWidth = 0.5f*terrainWidth;
	float halfHeight = 0.5f*terrainHeight;

	float dx = (float)terrainWidth /(n-1);
	float dz = (float)terrainHeight /(m-1);

	gridSize = dx;

	UINT index;

	for (UINT i = 0; i < terrainHeight; i++)
	{
		float z = halfHeight - (float)i * dz;

		for (UINT j = 0; j < terrainWidth; j++)
		{
			float x = -halfWidth + (float)j * dx;
			index = i * n + j;

			vertexList[index].setPos(dx::XMFLOAT3(x, heightMap[index].y, z));
			actualHeight[index] = heightMap[index].y;
			vertexList[index].setNormal(dx::XMFLOAT3(0.0f,1.0f,0.0f));
			vertexList[index].setUV(dx::XMFLOAT2((float)j, (float)i));
		}
	}
	
	int indicesIndex = 0;

	for (UINT i = 0; i < m-1; i++)
	{
		for (UINT j = 0; j < n-1; j++)
		{
			indicesList[indicesIndex] = i * n + j;
			indicesList[indicesIndex + 1] = i * n + j + 1;
			indicesList[indicesIndex + 2] = (i + 1)*n + j;
			indicesList[indicesIndex + 3] = (i + 1)*n + j;
			indicesList[indicesIndex + 4] = i * n + j + 1;
			indicesList[indicesIndex + 5] = (i + 1)*n + j + 1;
			indicesIndex += 6; // next quad
		}
	}
}

void Terrain::generatePlane(UINT width, UINT height)
{
	terrainHeight = height;
	terrainWidth = width;

	numberOfFaces = 0;
	numberOfVertices = 0;

	numberOfVertices = width * height;
	numberOfFaces = (width - 1) * (height - 1) * 2;

	vertexList.resize(numberOfVertices);
	vertexCount = (UINT)vertexList.size();

	indicesList.resize(numberOfFaces * 3);
	indicesCount = (UINT)indicesList.size();

	actualHeight.resize(numberOfVertices);

	float halfWidth = 0.5f*terrainWidth;
	float halfHeight = 0.5f*terrainHeight;

	float dx = (float)terrainWidth / (width - 1);
	float dz = (float)terrainHeight / (height - 1);

	gridSize = dx;

	UINT index;

	float heightMap = 0;

	for (UINT i = 0; i < terrainHeight; i++)
	{
		float z = halfHeight - (float)i * dz;

		for (UINT j = 0; j < terrainWidth; j++)
		{
			float x = -halfWidth + (float)j * dx;
			index = i * width + j;

			vertexList[index].setPos(dx::XMFLOAT3(x, heightMap, z));
			vertexList[index].setNormal(dx::XMFLOAT3(0.0f, 1.0f, 0.0f));
			vertexList[index].setUV(dx::XMFLOAT2((float)j, (float)i));
			actualHeight[index] = heightMap;
		}
	}

	int indicesIndex = 0;

	for (UINT i = 0; i < height - 1; i++)
	{
		for (UINT j = 0; j < width - 1; j++)
		{
			indicesList[indicesIndex] = i * width + j;
			indicesList[indicesIndex + 1] = i * width + j + 1;
			indicesList[indicesIndex + 2] = (i + 1)*width + j;
			indicesList[indicesIndex + 3] = (i + 1)*width + j;
			indicesList[indicesIndex + 4] = i * width + j + 1;
			indicesList[indicesIndex + 5] = (i + 1)*width + j + 1;
			indicesIndex += 6; // next quad
		}
	}
}

void Terrain::createVertexAndIndexBuffers()
{
	unsigned int vertexDataArrayByteSize = unsigned int(sizeof(Vertex) * vertexList.size());
	
	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.ByteWidth = vertexDataArrayByteSize;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT; // helps the driver optimisation, based on how often the buffer values will change
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; //binding a buffer of type to the BindFlags
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 }; // points to our array of floats
	vertexBufferData.pSysMem = vertexList.data();
	hr = devicePtr->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertexBuffer);
	assert(SUCCEEDED(hr));//gives us a pointer to a vertex buffer.

	//index
	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * numberOfFaces * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData = {};
	iinitData.pSysMem = &indicesList[0];
	//iinitData.pSysMem = indices.data();
	hr = devicePtr->CreateBuffer(&indexBufferDesc, &iinitData, &indexBuffer);
	assert(SUCCEEDED(hr));
}


bool Terrain::loadHeightMapSTB(std::string filename, float heightFactor)
{
	int x; // width
	int y; // height
	int n; // 8 bit
	int numberOfDesiredChannel = 1;
	unsigned char* data = stbi_load(filename.c_str(), &x, &y, &n, numberOfDesiredChannel);

	if (data != nullptr)
	{
		terrainHeight = y;
		terrainWidth = x;

		heightMap.resize(terrainWidth * terrainHeight);
		
		unsigned char height;
		int index = 0;
		int imageIndex = 0;

		for (UINT i = 0; i < terrainHeight; i++)
		{
			for (UINT x = 0; x < terrainWidth; x++)
			{
				height = data[imageIndex];
				index = (terrainHeight * i) + x;
				heightMap[index].x = (float)i;
				heightMap[index].y = (float)height / heightFactor;
				heightMap[index].z = (float)x;

				imageIndex = imageIndex + 1;
			}
		}
	}

	stbi_image_free(data);

	return (data != nullptr);
}

void Terrain::release()
{
	if (vertexBuffer != nullptr)
	{
		vertexBuffer->Release();
		vertexBuffer = nullptr;
	}

	if (indexBuffer != nullptr)
	{
		indexBuffer->Release();
		indexBuffer = nullptr;
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

void Terrain::drawDeferred(Camera & camera, Light & light, ID3D11DeviceContext *& deviceContextPtr, ID3D11SamplerState *& samplerStatePtr, ID3D11Buffer *& constantBufferPerObjectPtr, ConstantBuffer & constantBuffer, ID3D11Buffer *& constantBufferPtrMaterial, MaterialBuffer & material, dx::XMFLOAT4 speed)
{
	constantBuffer.world = dx::XMMatrixTranspose(worldMatrix);
	constantBuffer.WVP = dx::XMMatrixTranspose(worldMatrix * camera.getViewMatrix() * camera.getProjectionMatrix());
	constantBuffer.lightViewMatrix = dx::XMMatrixTranspose(light.getCamera().getViewMatrix());
	constantBuffer.lightProjectionMatrix = dx::XMMatrixTranspose(light.getCamera().getOrthographicMatrix());
	constantBuffer.cameraPos = camera.getPositionFloat();
	constantBuffer.hasANormalMap = FALSE;
	constantBuffer.animationSpeed = speed;

	deviceContextPtr->VSSetConstantBuffers(0, 1, &constantBufferPerObjectPtr);
	deviceContextPtr->PSSetConstantBuffers(0, 1, &constantBufferPerObjectPtr);
	deviceContextPtr->UpdateSubresource(constantBufferPerObjectPtr, 0, nullptr, &constantBuffer, 0, 0);

	deviceContextPtr->PSSetConstantBuffers(1, 1, &constantBufferPtrMaterial);
	deviceContextPtr->UpdateSubresource(constantBufferPtrMaterial, 0, nullptr, &material, 0, 0);

	deviceContextPtr->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexStride, &vertexOffset);
	deviceContextPtr->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContextPtr->PSSetShaderResources(0, 1, &textureResourceViewPtr);
	deviceContextPtr->PSSetShaderResources(1, 1, &normalTextureResourceViewPtr);

	deviceContextPtr->PSSetSamplers(0, 1, &samplerStatePtr);

	deviceContextPtr->DrawIndexed(indicesCount, 0, 0);
}

void Terrain::drawDeferred(Camera & camera, Light & light, ID3D11DeviceContext *& deviceContextPtr, ID3D11GeometryShader *& geometryShaderPtr, ID3D11SamplerState *& samplerStatePtr, ID3D11Buffer *& constantBufferPerObjectPtr, ConstantBuffer & constantBuffer, ID3D11Buffer *& constantBufferPtrMaterial, MaterialBuffer & material, ID3D11Buffer *& constantBufferPtrBackFaceCulling, GeometryShaderBuffer & geometryShaderBuffer)
{
	constantBuffer.world = dx::XMMatrixTranspose(worldMatrix);
	constantBuffer.WVP = dx::XMMatrixTranspose(worldMatrix * camera.getViewMatrix() * camera.getProjectionMatrix());

	constantBuffer.lightViewMatrix = dx::XMMatrixTranspose(light.getCamera().getViewMatrix());
	constantBuffer.lightProjectionMatrix = dx::XMMatrixTranspose(light.getCamera().getOrthographicMatrix());
	constantBuffer.cameraPos = camera.getPositionFloat();
	constantBuffer.hasANormalMap = FALSE;
	constantBuffer.animationSpeed = dx::XMFLOAT4(0,0,0,0);

	deviceContextPtr->VSSetConstantBuffers(0, 1, &constantBufferPerObjectPtr);
	deviceContextPtr->PSSetConstantBuffers(0, 1, &constantBufferPerObjectPtr);
	deviceContextPtr->UpdateSubresource(constantBufferPerObjectPtr, 0, nullptr, &constantBuffer, 0, 0);

	geometryShaderBuffer.cameraPos = camera.getPositionFloat();
	deviceContextPtr->GSSetConstantBuffers(0, 1, &constantBufferPtrBackFaceCulling);
	deviceContextPtr->UpdateSubresource(constantBufferPtrBackFaceCulling, 0, nullptr, &geometryShaderBuffer, 0, 0);

	deviceContextPtr->PSSetConstantBuffers(1, 1, &constantBufferPtrMaterial);
	deviceContextPtr->UpdateSubresource(constantBufferPtrMaterial, 0, nullptr, &material, 0, 0);

	deviceContextPtr->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexStride, &vertexOffset);
	deviceContextPtr->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContextPtr->GSSetShader(geometryShaderPtr, nullptr, 0);

	deviceContextPtr->PSSetShaderResources(0, 1, &textureResourceViewPtr);
	deviceContextPtr->PSSetShaderResources(1, 1, &normalTextureResourceViewPtr);

	deviceContextPtr->PSSetSamplers(0, 1, &samplerStatePtr);

	deviceContextPtr->DrawIndexed(indicesCount, 0, 0);
	deviceContextPtr->GSSetShader(nullptr, nullptr, 0);
}

void Terrain::drawShadow(Camera & camera, Light & light, ID3D11DeviceContext *& deviceContextPtr, ID3D11VertexShader *& vertexShaderPtr, ID3D11Buffer *& constantBufferPerObjectPtr, ConstantBuffer & constantBuffer)
{
	constantBuffer.world = dx::XMMatrixTranspose(worldMatrix);
	constantBuffer.WVP = dx::XMMatrixTranspose((worldMatrix * light.getCamera().getViewMatrix() *  light.getCamera().getOrthographicMatrix()));
	constantBuffer.lightViewMatrix = dx::XMMatrixTranspose(light.getCamera().getViewMatrix());
	constantBuffer.lightProjectionMatrix = dx::XMMatrixTranspose(light.getCamera().getOrthographicMatrix());
	constantBuffer.cameraPos = camera.getPositionFloat();
	constantBuffer.hasANormalMap = FALSE;
	constantBuffer.animationSpeed = dx::XMFLOAT4(0, 0, 0, 0);

	deviceContextPtr->VSSetConstantBuffers(0, 1, &constantBufferPerObjectPtr);
	deviceContextPtr->UpdateSubresource(constantBufferPerObjectPtr, 0, nullptr, &constantBuffer, 0, 0);
	deviceContextPtr->VSSetShader(vertexShaderPtr, nullptr, 0);
	deviceContextPtr->PSSetShader(nullptr, nullptr, 0);
	deviceContextPtr->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexStride, &vertexOffset);
	deviceContextPtr->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	
	deviceContextPtr->DrawIndexed(indicesCount, 0, 0);
}

void Terrain::readTextureFromFile(std::wstring fileName)
{
	hr = dx::CreateWICTextureFromFile(devicePtr, fileName.data(), nullptr, &textureResourceViewPtr);
	assert(SUCCEEDED(hr));
}

void Terrain::readNormalTextureFromFile(std::wstring fileName)
{
	hr = dx::CreateWICTextureFromFile(devicePtr, fileName.data(), nullptr, &normalTextureResourceViewPtr);
	assert(SUCCEEDED(hr));
}

dx::XMFLOAT3 Terrain::getTranslation() const
{
	return this->translation;
}

dx::XMFLOAT3 Terrain::getRotation() const
{
	return this->rotation;
}

dx::XMFLOAT3 Terrain::getScale() const
{
	return this->scale;
}

void Terrain::setTranslation(dx::XMFLOAT3 translation)
{
	this->translation = translation;
	translationMatrix = dx::XMMatrixTranslation(this->translation.x, this->translation.y, this->translation.z);
	setWorldMatrix();
}

void Terrain::setRotation(dx::XMFLOAT3 rotation)
{
	this->rotation = rotation;
	rotationMatrix = dx::XMMatrixRotationRollPitchYaw(this->rotation.x, this->rotation.y, this->rotation.z);
	setWorldMatrix();
}

void Terrain::setScale(dx::XMFLOAT3 scale)
{
	this->scale = scale;
	scaleMatrix = dx::XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
	setWorldMatrix();
}

void Terrain::setWorldMatrix()
{
	this->worldMatrix = this->scaleMatrix * this->rotationMatrix * this->translationMatrix;
}

void Terrain::setWorldMatrix(dx::XMMATRIX matrix)
{
	this->worldMatrix = matrix;
}

dx::XMMATRIX Terrain::getTranslationMatrix() const
{
	return this->translationMatrix;
}

dx::XMMATRIX Terrain::getRotationMatrix() const
{
	return this->rotationMatrix;
}

dx::XMMATRIX Terrain::getScaleMatrix() const
{
	return this->scaleMatrix;
}

dx::XMMATRIX Terrain::getWorldMatrix() const
{
	return this->worldMatrix;
}

std::vector<Vertex> &Terrain::getVertexList()
{
	return this->vertexList;
}

std::vector<UINT> &Terrain::getIndicesList()
{
	return this->indicesList;
}

UINT Terrain::getVertexCount() const
{
	return this->vertexCount;
}

UINT &Terrain::getVertexStride()
{
	return this->vertexStride;
}

UINT &Terrain::getVertexOffset()
{
	return this->vertexOffset;
}

UINT Terrain::getIndicesCount()  const
{
	return this->indicesCount;
}

Material &Terrain::getMaterial() 
{
	return this->material;
}

void Terrain::setMaterial(Material material)
{
	this->material = material;
}

float Terrain::getTerrainHeight(float x, float z)
{
	float heightValue = 0;

	float offsetX = x - translation.x;
	float offsetZ = z - translation.z;

	// Transform from local space to "cell" space.
	float c = (offsetX + 0.5f * terrainWidth) / gridSize;
	float d = (offsetZ - 0.5f * terrainHeight) / -gridSize;
	
	// Get the row and column we are in.
	int row = (int)floor(d);
	int col = (int)floor(c);

	// Checks if we are outside the terrain
	if (row < 0 || row > (int)terrainHeight - 2)
	{
		return 0;
	}
	else if(col < 0 || col >(int)terrainWidth - 2)
	{
		return 0;
	}

	// Takes the heights of the cell we are in.
	float PointA = actualHeight[row * terrainWidth + col];
	float PointB = actualHeight[row * terrainWidth + col + 1];
	float PointC = actualHeight[(row+1) * terrainWidth + col];;
	float PointD = actualHeight[(row + 1) * terrainWidth + col + 1];

	// Where we are relative to the cell.
	float s = c - (float)col;
	float t = d - (float)row;

	// If upper triangle ABC.
	if (s + t <= 1.0f)
	{
		float uy = PointB - PointA;
		float vy = PointC - PointA;

		heightValue = PointA + s * uy + t * vy;
	}
	else // lower triangle DCB.
	{
		float uy = PointC - PointD;
		float vy = PointB - PointD;

		heightValue = PointD + (1.0f - s)  * uy + (1.0f-t) * vy;
	}

	return heightValue;
}

