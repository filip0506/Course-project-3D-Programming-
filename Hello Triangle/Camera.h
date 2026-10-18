#pragma once
#include "MyIncludes.h"
#include <iostream>
#include "Vertex.h"
#include <vector>

namespace dx = DirectX;

static const dx::XMVECTOR defaultForwardVector = dx::XMVectorSet(0, 0, 1, 0);
static const dx::XMVECTOR defaultBackVector = dx::XMVectorSet(0, 0, -1, 0);

static const dx::XMVECTOR defaultUpVector = dx::XMVectorSet(0, 1, 0, 0);
static const dx::XMVECTOR defaultDownVector = dx::XMVectorSet(0, -1, 0, 0);

static const dx::XMVECTOR defaultRightVector = dx::XMVectorSet(1, 0, 0, 0);
static const dx::XMVECTOR defaultLeftVector = dx::XMVectorSet(-1, 0, 0, 0);

class Camera
{
private:

	dx::XMVECTOR forwardVector = dx::XMVectorSet(0, 0, 1, 0);
	dx::XMVECTOR upVector = dx::XMVectorSet(0, 1, 0, 0);
	dx::XMVECTOR rightVector = dx::XMVectorSet(1, 0, 0, 0);

	dx::XMVECTOR position;
	dx::XMVECTOR target;
	dx::XMVECTOR up;
	dx::XMVECTOR rotation;
	dx::XMFLOAT3 positionFloat;
	dx::XMFLOAT3 rotationFloat;

	dx::XMFLOAT3 scale = dx::XMFLOAT3(1, 1, 1);

	float fieldOfView;

	float aspectRatio;
	float nearZ;
	float farZ;

	dx::XMMATRIX worldSpace = dx::XMMatrixIdentity();
	dx::XMMATRIX viewMatrix = dx::XMMatrixIdentity();
	dx::XMMATRIX projectionMatrix = dx::XMMatrixIdentity();
	dx::XMMATRIX orthographicMatrix = dx::XMMatrixIdentity();
	dx::XMMATRIX rotationMatrix = dx::XMMatrixIdentity();

	dx::XMFLOAT4X4 projectionMatrix4X4;
	dx::XMFLOAT4X4 viewMatrix4X4;
	float moveLeftRight = 0;
	float moveBackForward = 0;
	float yaw = 0.0f;
	float pitch = 0.0f;

	float cameraOffset = 1;
	bool collision = false;
			
public:

	Camera();
	Camera(dx::XMFLOAT4 pos, dx::XMFLOAT4 tar, dx::XMFLOAT4 up);

	dx::XMVECTOR getPosition() const;
	void setPosition(dx::XMFLOAT4 pos);

	dx::XMFLOAT3 getPositionFloat() const;
	void setPositionFloat(dx::XMFLOAT3 pos);

	dx::XMVECTOR getTarget() const;
	void setTarget(dx::XMFLOAT4 target);

	dx::XMVECTOR getUp() const;
	void setUp(dx::XMFLOAT4 up);

	dx::XMVECTOR getRotation() const;
	void setRotation(dx::XMFLOAT4 rot);

	float getFieldOfView() const;
	void setFieldOfView(float fov);

	float getAspectRatio() const;
	void setAspectRatio(float width, float height);
		
	float getNearZ() const;
	void setNearZ(float nearZ);
		
	float getFarZ() const;
	void setFarZ(float farZ);
		
	float getPitch() const;
	void setPitch(float pitch);
		
	float getYaw() const;
	void setYaw(float yaw);

	float getOffset() const;
	void setOffset(float offset);

	void setWorldMatrix();
	dx::XMMATRIX getWorldMatrix() const;

	dx::XMMATRIX getViewMatrix() const;
	void setViewMatrix(dx::XMVECTOR pos, dx::XMVECTOR target, dx::XMVECTOR up);
	void setViewMatrix();


	dx::XMMATRIX getProjectionMatrix() const;
	void setProjectionMatrix(float fieldOfView, float aspectRatio, float nearZ, float farZ);
	void setProjectionMatrix();

	dx::XMMATRIX getOrthographicMatrix() const;
	void setOrthographicMatrix(float width, float height, float nearZ, float farZ);

	void setEverythingProjection(float windowWidth, float windowHeight, float width, float height, float nearPlane, float farPlane, float fieldOfView);
	dx::XMFLOAT3 cameraToTargetVector();

	dx::XMFLOAT4X4 getProjectionMatrix4X4();
	dx::XMFLOAT4X4 getviewMatrix4X4();

	void update();

	void updateProjectionMatrix();
	void move(dx::XMFLOAT4 pos);

	void changeRotation(dx::XMFLOAT4 rot);

	bool getCollisionBool() const;
	void setCollisionBool(float collision);
		
};