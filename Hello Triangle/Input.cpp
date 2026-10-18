#include "Input.h"



Input::Input(HINSTANCE & hInstance, HWND & hwnd, float windowWidth, float windowHeight)
{
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;

	hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(hr));

	//keyboard
	hr = directInput->CreateDevice(GUID_SysKeyboard, &IDirectKeyBoardPtr, nullptr);
	hr = IDirectKeyBoardPtr->SetDataFormat(&c_dfDIKeyboard);
	hr = IDirectKeyBoardPtr->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	//mouse
	hr = directInput->CreateDevice(GUID_SysMouse, &IDirectMousePtr, nullptr);
	hr = IDirectMousePtr->SetDataFormat(&c_dfDIMouse);
	hr = IDirectMousePtr->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	//hr = IDirectMousePtr->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

	POINT mousePos;
	mousePos.x = (LONG)this->windowWidth / 2;
	mousePos.y = (LONG)this->windowHeight / 2;
	ClientToScreen(hwnd, &mousePos);
	SetCursorPos(mousePos.x, mousePos.y);

}

void Input::mouseInput(Camera & camera, HWND & hwnd, int windowWidth, int windowHeight, std::vector<Mesh*> &scene)
{
	static float xDir = 0;
	static float yDir = 0;

	static float ViewSpaceX = 1;
	static float ViewSpaceY = 1;
	static float ViewSpaceZ = 0;

	static dx::XMVECTOR U;
	static dx::XMVECTOR V;

	DIMOUSESTATE currentMouseState;
	IDirectMousePtr->Acquire();
	IDirectMousePtr->GetDeviceState(sizeof(DIMOUSESTATE), &currentMouseState);

	int index = 0;

	if (currentMouseState.rgbButtons[0])
	{
		if (pressedDown == false)
		{
			POINT mousePos;
			GetCursorPos(&mousePos);
			ScreenToClient(hwnd, &mousePos);

			float mousePosX = (float)mousePos.x;
			float mousePosY = (float)mousePos.y;

			float tempDistance = 0;
			float closestDistance = FLT_MAX;

			transformToWorldSpace(mousePosX, mousePosY, camera);

			for (size_t i = 0; i < scene.size(); i++)
			{
				tempDistance = loopThroughVertexPoint(camera, scene[i]->getWorldMatrix(), (UINT)i, scene);

				if (tempDistance < closestDistance)
				{
					closestDistance = tempDistance;
					index = i;
				}

			}

			if (closestDistance < FLT_MAX)
			{
				pickedDistance = closestDistance;
				scene[index]->setTranslation(dx::XMFLOAT3(scene[index]->getTranslation().x + 1, 0, 0));				
			}
		}

		pressedDown = true;
	}

	if (!currentMouseState.rgbButtons[0] && pressedDown == true)
	{
		pressedDown = false;
	}

	if (currentMouseState.lX != lastMouseState.lX)
	{
		xDir -= (currentMouseState.lX * -0.001f);
		camera.setYaw(xDir);
	}

	if (currentMouseState.lY != lastMouseState.lY)
	{
		yDir -= (currentMouseState.lY * -0.001f);

		if (yDir >= 1.5707f)
		{
			yDir = 1.5707f;
		}
		else if (yDir <= -1.5707f)
		{
			yDir = -1.5707f;
		}

		camera.setPitch(yDir);
	}

	lastMouseState = currentMouseState;
}

void Input::mouseInput(Camera & camera, HWND & hwnd, std::vector<Mesh*>& scene, GameManager & gamemanager)
{
	static float xDirection = 0;
	static float yDirection = 0;

	DIMOUSESTATE currentMouseState;

	IDirectMousePtr->Acquire();
	IDirectMousePtr->GetDeviceState(sizeof(DIMOUSESTATE), &currentMouseState);

	int index = 0;

	if (currentMouseState.rgbButtons[0])
	{
		if (pressedDown == false)
		{
			POINT mousePos;
			GetCursorPos(&mousePos);
			ScreenToClient(hwnd, &mousePos);

			float mousePositionX = (float)mousePos.x;
			float mousePositionY = (float)mousePos.y;

			float tempDistance = 0;
			float closestDistance = FLT_MAX;
		
			transformToWorldSpace(mousePositionX, mousePositionY, camera);

			for (size_t i = 0; i < scene.size(); i++)
			{
				if (scene[i]->getPickable() == true)
				{
					tempDistance = loopThroughVertexPoint(camera, scene[i]->getWorldMatrix(), (UINT)i, scene);

					if (tempDistance < closestDistance)
					{
						closestDistance = tempDistance;
						index = i;
					}
				}
			}

			if (closestDistance < FLT_MAX)
			{
				pickedDistance = closestDistance;

				if (scene[index]->getPickable() == true)
				{
					scene[index]->setTranslation(dx::XMFLOAT3(scene[index]->getTranslation().x, -4, scene[index]->getTranslation().z));
					scene[index]->setPickable(false);
					gamemanager.addScore(100);
				}

				gamemanager.checkIfGameOver(scene);

				std::cout << gamemanager.getScore() << std::endl;
			}

		}

		pressedDown = true;
	}

	if (!currentMouseState.rgbButtons[0] && pressedDown == true)
	{
		pressedDown = false;
	}

	if (currentMouseState.rgbButtons[1])
	{
		POINT mousePos;
		mousePos.x = (LONG)this->windowWidth / 2;
		mousePos.y = (LONG)this->windowHeight / 2;
		ClientToScreen(hwnd, &mousePos);
		SetCursorPos(mousePos.x, mousePos.y);
	}

	if (currentMouseState.lX != lastMouseState.lX)
	{
		xDirection -= (currentMouseState.lX * -0.001f);
		camera.setYaw(xDirection);
	}

	if (currentMouseState.lY != lastMouseState.lY)
	{
		yDirection -= (currentMouseState.lY * -0.001f);

		if (yDirection >= 1.5707f)
		{
			yDirection = 1.5707f;
		}
		else if (yDirection <= -1.5707f)
		{
			yDirection = -1.5707f;

		}

		camera.setPitch(yDirection);
	}

	lastMouseState = currentMouseState;
}

void Input::keyboardInput(Camera & camera, Light & light, ConstantBuffer & cbuff, Mesh & mesh)
{
	unsigned char keyBoardState[256];

	IDirectKeyBoardPtr->Acquire();
	IDirectKeyBoardPtr->GetDeviceState(sizeof(keyBoardState), (LPVOID)&keyBoardState);

	float height = 0;

	if (keyBoardState[DIK_LEFT] & 0x80)
	{
		camera.move(dx::XMFLOAT4(-speed, 0.0f, 0.0f, 0.0f));

		if (camera.getPositionFloat().y < height + camera.getOffset())
		{
			camera.setPosition(dx::XMFLOAT4(camera.getPositionFloat().x, height + camera.getOffset(), camera.getPositionFloat().z, 0));
		}
	}

	if (keyBoardState[DIK_RIGHT] & 0x80)
	{
		camera.move(dx::XMFLOAT4(speed, 0.0f, 0.0f, 0.0f));

		if (camera.getPositionFloat().y < height + camera.getOffset())
		{
			camera.setPosition(dx::XMFLOAT4(camera.getPositionFloat().x, height + camera.getOffset(), camera.getPositionFloat().z, 0));
		}
	}

	if (keyBoardState[DIK_UP] & 0x80)
	{
		camera.move(dx::XMFLOAT4(0.0f, 0.0f, speed, 0.0f));

		if (camera.getPositionFloat().y < height + camera.getOffset())
		{
			camera.setPosition(dx::XMFLOAT4(camera.getPositionFloat().x, height + camera.getOffset(), camera.getPositionFloat().z, 0));
		}
	}

	if (keyBoardState[DIK_DOWN] & 0x80)
	{
		camera.move(dx::XMFLOAT4(0.0f, 0.0f, -speed, 0.0f));

		if (camera.getPositionFloat().y < height + camera.getOffset())
		{
			camera.setPosition(dx::XMFLOAT4(camera.getPositionFloat().x, height + camera.getOffset(), camera.getPositionFloat().z, 0));
		}
	}

	if (keyBoardState[DIK_SPACE] & 0x80)
	{
		camera.move(dx::XMFLOAT4(0.0f, 0.05f, 0.0f, 0.0f));

		if (camera.getPositionFloat().y < height + camera.getOffset())
		{
			camera.setPosition(dx::XMFLOAT4(camera.getPositionFloat().x, height + camera.getOffset(), camera.getPositionFloat().z, 0));
		}
	}

	if (keyBoardState[DIK_LSHIFT] & 0x80)
	{
		camera.move(dx::XMFLOAT4(0.0f, -speed, 0.0f, 0.0f));

		if (camera.getPositionFloat().y < height + camera.getOffset())
		{
			camera.setPosition(dx::XMFLOAT4(camera.getPositionFloat().x, height + camera.getOffset(), camera.getPositionFloat().z, 0));
		}
	}

	if (keyBoardState[DIK_ESCAPE] & 0x80)
	{
		PostQuitMessage(0);
	}

	if (keyBoardState[DIK_Q] & 0x80)
	{
		light.setPos(dx::XMFLOAT3(10, 10, 0));
	}

	if (keyBoardState[DIK_W] & 0x80)
	{
		light.setPos(dx::XMFLOAT3(light.getPos().x, (light.getPos().y), light.getPos().z + 0.05f));
	}

	if (keyBoardState[DIK_S] & 0x80)
	{
		light.setPos(dx::XMFLOAT3(light.getPos().x, (light.getPos().y), light.getPos().z - 0.05f));
	}

	if (keyBoardState[DIK_D] & 0x80)
	{
		light.setPos(dx::XMFLOAT3(light.getPos().x + 0.05f, (light.getPos().y), light.getPos().z));
	}

	if (keyBoardState[DIK_A] & 0x80)
	{
		light.setPos(dx::XMFLOAT3(light.getPos().x - 0.05f, (light.getPos().y), light.getPos().z));
	}

	if (keyBoardState[DIK_G] & 0x80)
	{
		if (mesh.hasNormalMap() == FALSE)
		{
			mesh.setHasNormalMap(TRUE);
		}
		else if (mesh.hasNormalMap() == TRUE)
		{
			mesh.setHasNormalMap(FALSE);
		}

		mesh.setHasNormalMap(TRUE);
	}

	if (keyBoardState[DIK_H] & 0x80)
	{
		if (mesh.hasNormalMap() == FALSE)
		{
			mesh.setHasNormalMap(TRUE);
		}
		else if (mesh.hasNormalMap() == TRUE)
		{
			mesh.setHasNormalMap(FALSE);
		}

		mesh.setHasNormalMap(FALSE);
	}

	if (keyBoardState[DIK_Z] & 0x80)
	{
		light.setPos(dx::XMFLOAT3((light.getPos().x), light.getPos().y - 0.05f, light.getPos().z));
	}

	if (keyBoardState[DIK_X] & 0x80)
	{
		light.setPos(dx::XMFLOAT3(light.getPos().x, light.getPos().y + 0.05f, light.getPos().z));
	}

	if (keyBoardState[DIK_V] & 0x80)
	{
		mesh.setTranslation(dx::XMFLOAT3(mesh.getTranslation().x - 0.05f, mesh.getTranslation().y, mesh.getTranslation().z));
	}

	if (keyBoardState[DIK_B] & 0x80)
	{
		mesh.setTranslation(dx::XMFLOAT3(mesh.getTranslation().x + 0.05f, mesh.getTranslation().y, mesh.getTranslation().z));
	}

	if (keyBoardState[DIK_C] & 0x80)
	{
		mesh.setRotation(dx::XMFLOAT3(mesh.getRotation().x, mesh.getRotation().y, mesh.getRotation().z + 0.05f));
	}

	if (keyBoardState[DIK_E] & 0x80)
	{
		light.setPos(dx::XMFLOAT3(-10, 10, 0));
	}
}

void Input::keyboardInput(Camera & camera, Light & light, ConstantBuffer & cbuff, Mesh & mesh, Terrain & terrain)
{
	unsigned char keyBoardState[256];

	IDirectKeyBoardPtr->Acquire();
	IDirectKeyBoardPtr->GetDeviceState(sizeof(keyBoardState), (LPVOID)&keyBoardState);

	float height = terrain.getTerrainHeight(camera.getPositionFloat().x, camera.getPositionFloat().z);

	if (keyBoardState[DIK_LEFT] & 0x80)
	{
		camera.move(dx::XMFLOAT4(-speed, 0.0f, 0.0f, 0.0f));
		terrainCollisonCheck(camera, height);
	}

	if (keyBoardState[DIK_RIGHT] & 0x80)
	{
		camera.move(dx::XMFLOAT4(speed, 0.0f, 0.0f, 0.0f));
		terrainCollisonCheck(camera, height);
	}

	if (keyBoardState[DIK_UP] & 0x80)
	{
		camera.move(dx::XMFLOAT4(0.0f, 0.0f, speed, 0.0f));
		terrainCollisonCheck(camera, height);
	}

	if (keyBoardState[DIK_DOWN] & 0x80)
	{
		camera.move(dx::XMFLOAT4(0.0f, 0.0f, -speed, 0.0f));
		terrainCollisonCheck(camera, height);
	}

	if (keyBoardState[DIK_SPACE] & 0x80)
	{
		camera.move(dx::XMFLOAT4(0.0f, 0.05f, 0.0f, 0.0f));
		terrainCollisonCheck(camera, height);
	}

	if (keyBoardState[DIK_LSHIFT] & 0x80)
	{
		camera.move(dx::XMFLOAT4(0.0f, -speed, 0.0f, 0.0f));
		terrainCollisonCheck(camera, height);
	}

	if (keyBoardState[DIK_ESCAPE] & 0x80)
	{
		PostQuitMessage(0);
	}

	if (keyBoardState[DIK_Q] & 0x80)
	{
		light.setPos(dx::XMFLOAT3(10, 10, 0));
	}

	if (keyBoardState[DIK_E] & 0x80)
	{
		light.setPos(dx::XMFLOAT3(-10, 10, 0));
	}

	if (keyBoardState[DIK_W] & 0x80)
	{
		light.setPos(dx::XMFLOAT3(light.getPos().x, (light.getPos().y), light.getPos().z + 0.05f));
	}

	if (keyBoardState[DIK_S] & 0x80)
	{
		light.setPos(dx::XMFLOAT3(light.getPos().x, (light.getPos().y), light.getPos().z - 0.05f));
	}

	if (keyBoardState[DIK_D] & 0x80)
	{
		light.setPos(dx::XMFLOAT3(light.getPos().x + 0.05f, (light.getPos().y), light.getPos().z));
	}

	if (keyBoardState[DIK_A] & 0x80)
	{
		light.setPos(dx::XMFLOAT3(light.getPos().x - 0.05f, (light.getPos().y), light.getPos().z));
	}

	if (keyBoardState[DIK_Z] & 0x80)
	{
		light.setPos(dx::XMFLOAT3((light.getPos().x), light.getPos().y - 0.05f, light.getPos().z));
	}

	if (keyBoardState[DIK_X] & 0x80)
	{
		light.setPos(dx::XMFLOAT3(light.getPos().x, light.getPos().y + 0.05f, light.getPos().z));
	}

	if (keyBoardState[DIK_G] & 0x80)
	{
		if (mesh.hasNormalMap() == FALSE)
		{
			mesh.setHasNormalMap(TRUE);
		}
		else if (mesh.hasNormalMap() == TRUE)
		{
			mesh.setHasNormalMap(FALSE);
		}

		mesh.setHasNormalMap(TRUE);
	}

	if (keyBoardState[DIK_H] & 0x80)
	{
		if (mesh.hasNormalMap() == FALSE)
		{
			mesh.setHasNormalMap(TRUE);
		}
		else if (mesh.hasNormalMap() == TRUE)
		{
			mesh.setHasNormalMap(FALSE);
		}

		mesh.setHasNormalMap(FALSE);
	}

	if (keyBoardState[DIK_V] & 0x80)
	{
		mesh.setTranslation(dx::XMFLOAT3(mesh.getTranslation().x - 0.05f, mesh.getTranslation().y, mesh.getTranslation().z));
	}

	if (keyBoardState[DIK_B] & 0x80)
	{
		mesh.setTranslation(dx::XMFLOAT3(mesh.getTranslation().x + 0.05f, mesh.getTranslation().y, mesh.getTranslation().z));
	}

	if (keyBoardState[DIK_C] & 0x80)
	{
		mesh.setRotation(dx::XMFLOAT3(mesh.getRotation().x, mesh.getRotation().y, mesh.getRotation().z + 0.05f));
	}

	if (keyBoardState[DIK_R] & 0x80)
	{
		camera.setCollisionBool(!camera.getCollisionBool());
	}
	
}

// Checks if the object intersects. If the object does not intersect the distance return is FLT_MAX
float Input::loopThroughVertexPoint(Camera & cam, dx::XMMATRIX worldMatrix, UINT index, std::vector<Mesh*> &scene)
{
	//Loop through each triangle in the object
	for (size_t x = 0; x < scene[index]->getVertexList().size(); x = x + 3)
	{
		// Triangle's vertices V1, V2, V3
		dx::XMFLOAT3 vertexPoint1 = scene[index]->getVertexList()[x].getPos();
		dx::XMFLOAT3 vertexPoint2 = scene[index]->getVertexList()[x + 1].getPos();
		dx::XMFLOAT3 vertexPoint3 = scene[index]->getVertexList()[x + 2].getPos();
		
		// triangle
		dx::XMVECTOR triangleVertex1 = dx::XMVectorSet(vertexPoint1.x, vertexPoint1.y, vertexPoint1.z, 0);
		dx::XMVECTOR triangleVertex2 = dx::XMVectorSet(vertexPoint2.x, vertexPoint2.y, vertexPoint2.z, 0);
		dx::XMVECTOR triangleVertex3 = dx::XMVectorSet(vertexPoint3.x, vertexPoint3.y, vertexPoint3.z, 0);

		// Transforming the vertices to world space
		triangleVertex1 = dx::XMVector3TransformCoord(triangleVertex1, worldMatrix);
		triangleVertex2 = dx::XMVector3TransformCoord(triangleVertex2, worldMatrix);
		triangleVertex3 = dx::XMVector3TransformCoord(triangleVertex3, worldMatrix);

		// Find the normal using U, V coordinates (two edges)
		dx::XMVECTOR U = dx::XMVectorSubtract(triangleVertex2, triangleVertex1);
		dx::XMVECTOR V = dx::XMVectorSubtract(triangleVertex3, triangleVertex1);

		// Compute face normal by crossing U, V
		dx::XMVECTOR faceNormal = dx::XMVector3Normalize(dx::XMVector3Cross(U, V));

		// Calculate a point on the triangle for the plane equation
		dx::XMVECTOR trianglePoint = triangleVertex1;

		// Get plane equation ("Ax + By + Cz + D = 0")
		float triangleA = dx::XMVectorGetX(faceNormal);
		float triangleB = dx::XMVectorGetY(faceNormal);
		float triangleC = dx::XMVectorGetZ(faceNormal);
		float triangleD = ((-triangleA * dx::XMVectorGetX(trianglePoint)) - (triangleB * dx::XMVectorGetY(trianglePoint)) - (triangleC * dx::XMVectorGetZ(trianglePoint)));

		// Point where it intersects with the triangles plane
		float t = 0.0f;

		float ep1 = (dx::XMVectorGetX(positionInWorldSpace)*triangleA) + (dx::XMVectorGetY(positionInWorldSpace) * triangleB) + (dx::XMVectorGetZ(positionInWorldSpace) * triangleC);
		float ep2 = (dx::XMVectorGetX(directionInWorldSpace)*triangleA) + (dx::XMVectorGetY(directionInWorldSpace) * triangleB) + (dx::XMVectorGetZ(directionInWorldSpace) * triangleC);

		// No divide-by-zeros
		if (ep2 != 0.0f)
		{
			t = -(ep1 + triangleD) / (ep2);
		}

		if (t > 0.0f) // No picking objects behind the camera
		{
			//Get the point on the plane
			float planeIntersectX = dx::XMVectorGetX(positionInWorldSpace) + dx::XMVectorGetX(directionInWorldSpace) * t;
			float planeIntersectY = dx::XMVectorGetY(positionInWorldSpace) + dx::XMVectorGetY(directionInWorldSpace) * t;
			float planeIntersectZ = dx::XMVectorGetZ(positionInWorldSpace) + dx::XMVectorGetZ(directionInWorldSpace) * t;

			dx::XMVECTOR pointInPlane = dx::XMVectorSet(planeIntersectX, planeIntersectY, planeIntersectZ, 0.0f);

			// Check if the point is in the triangle
			if (pointInTriangle(triangleVertex1, triangleVertex2, triangleVertex3, pointInPlane))
			{
				//Return the distance to the hit
				return t / 2.0f;
			}
		}
	}

	return FLT_MAX;
}

// Transforms the 2d positon into world space. 
void Input::transformToWorldSpace(float mousePositionX, float mousePositionY, Camera & camera)
{
	// Transform picked position in screen space to view space
	float viewSpaceX = (((2.0f * mousePositionX) / windowWidth) - 1) / camera.getProjectionMatrix4X4()._11;
	float viewSpaceY = -(((2.0f * mousePositionY) / windowHeight) - 1) / camera.getProjectionMatrix4X4()._22;
	float viewSpaceZ = 1.0f;

	dx::XMVECTOR directionInViewSpace = dx::XMVectorSet(viewSpaceX, viewSpaceY, viewSpaceZ, 0.0f);
	dx::XMVECTOR positionInViewSpace = dx::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	// Transform from View space to World space
	dx::XMMATRIX worldSpaceMatrix;
	dx::XMVECTOR matrixInversDeterminant; // XMMatrixInverse first parameter can´t be null

	worldSpaceMatrix = dx::XMMatrixInverse(&matrixInversDeterminant, camera.getViewMatrix());;    //Inverse of View Space matrix is World space matrix

	positionInWorldSpace = dx::XMVector3TransformCoord(positionInViewSpace, worldSpaceMatrix);
	directionInWorldSpace = dx::XMVector3TransformNormal(directionInViewSpace, worldSpaceMatrix);
	
}

// Checks if the point is inside the triangle
bool Input::pointInTriangle(dx::XMVECTOR & triangleVertex1, dx::XMVECTOR & triangleVertex2, dx::XMVECTOR & triangleVertex3, dx::XMVECTOR & pointInPlane)
{
	dx::XMVECTOR point1 = dx::XMVector3Cross((dx::XMVectorSubtract(triangleVertex3, triangleVertex2)), (dx::XMVectorSubtract(pointInPlane, triangleVertex2)));
	dx::XMVECTOR point2 = dx::XMVector3Cross((dx::XMVectorSubtract(triangleVertex3, triangleVertex2)), (dx::XMVectorSubtract(triangleVertex1, triangleVertex2)));
	
	if (dx::XMVectorGetX(dx::XMVector3Dot(point1, point2)) >= 0)
	{
		point1 = dx::XMVector3Cross(dx::XMVectorSubtract(triangleVertex3, triangleVertex1), dx::XMVectorSubtract(pointInPlane, triangleVertex1));
		point2 = dx::XMVector3Cross(dx::XMVectorSubtract(triangleVertex3, triangleVertex1), dx::XMVectorSubtract(triangleVertex2, triangleVertex1));

		if (dx::XMVectorGetX(dx::XMVector3Dot(point1, point2)) >= 0)
		{
			point1 = dx::XMVector3Cross(dx::XMVectorSubtract(triangleVertex2, triangleVertex1), dx::XMVectorSubtract(pointInPlane, triangleVertex1));
			point2 = dx::XMVector3Cross(dx::XMVectorSubtract(triangleVertex2, triangleVertex1), dx::XMVectorSubtract(triangleVertex3, triangleVertex1));
		
			if (dx::XMVectorGetX(dx::XMVector3Dot(point1, point2)) >= 0)
			{
				return true;
			}
			else 
			{
				return false;
			}
		}
		else 
		{
			return false;
		}
	}
	return false;
}

void Input::terrainCollisonCheck(Camera & camera, float height)
{
	if (camera.getCollisionBool() == true)
	{
		if (camera.getPositionFloat().y < height + camera.getOffset())
		{
			camera.setPosition(dx::XMFLOAT4(camera.getPositionFloat().x, height + camera.getOffset(), camera.getPositionFloat().z, 0));
		}
	}
}

void Input::release()
{
	IDirectKeyBoardPtr->Unacquire();
	IDirectKeyBoardPtr->Release();
	IDirectKeyBoardPtr = nullptr;
	IDirectMousePtr->Unacquire();
	IDirectMousePtr->Release();
	IDirectMousePtr = nullptr;
	directInput->Release();
	directInput = nullptr;
}
