#pragma once
#include <dinput.h>
#include "MyIncludes.h"
#include "Camera.h"
#include "Light.h"
#include "Mesh.h"
#include "Terrain.h"
#include "GameManager.h"

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

namespace dx = DirectX;

class Input 
{
private:

	IDirectInputDevice8 *IDirectKeyBoardPtr = nullptr;
	IDirectInputDevice8 *IDirectMousePtr = nullptr;
	DIMOUSESTATE lastMouseState;
	LPDIRECTINPUT8 directInput;
	HRESULT hr;
	
	bool pressedDown = false;
	float speed = 0.1f;
	float pickedDistance = 0.0f;
	bool disablePicking = false;

	float windowWidth = 0;
	float windowHeight = 0;

	dx::XMVECTOR positionInWorldSpace = dx::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	dx::XMVECTOR directionInWorldSpace = dx::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

public:

	Input(HINSTANCE &hInstance, HWND &hwnd, float windowWidth, float windowHeight);
	
	void mouseInput(Camera & camera, HWND & hwnd, int windowWidth, int windowHeight, std::vector<Mesh*> &scene);
	void mouseInput(Camera & camera, HWND & hwnd, std::vector<Mesh*> &scene, GameManager &gamemanager);

	void keyboardInput(Camera & camera, Light & light, ConstantBuffer & cbuff, Mesh &mesh);
	void keyboardInput(Camera & camera, Light & light, ConstantBuffer & cbuff, Mesh &mesh, Terrain &terrain);

	float loopThroughVertexPoint(Camera &cam, dx::XMMATRIX worldMatrix, UINT index, std::vector<Mesh*> &scene);
	
	void transformToWorldSpace(float mouseX, float mouseY, Camera &camera); // Transforms the 2d cursor positon into a 3d ray in world space. 
	
	bool pointInTriangle(dx::XMVECTOR & triV1, dx::XMVECTOR & triV2, dx::XMVECTOR & triV3, dx::XMVECTOR & point);
	
	void terrainCollisonCheck(Camera & camera, float height);
	
	void release();
};