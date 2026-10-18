#include "Camera.h"


Camera::Camera()
{
	this->position = dx::XMVectorSet(0, 0, 0, 0);
	this->target = dx::XMVectorSet(0, 0, 0, 0);
	this->up = dx::XMVectorSet(0, 0, 0, 0);
	this->rotation = dx::XMVectorSet(0, 0, 0, 0);
	this->positionFloat = dx::XMFLOAT3(0, 0, 0);
}

Camera::Camera(dx::XMFLOAT4 pos, dx::XMFLOAT4 tar, dx::XMFLOAT4 up)
{
	this->position = dx::XMVectorSet(pos.x, pos.y, pos.z, pos.w);
	this->target = dx::XMVectorSet(tar.x, tar.y, tar.z, tar.w);
	this->up = dx::XMVectorSet(up.x, up.y, up.z, up.w);
	this->rotation = dx::XMVectorSet(0, 0, 0, 0);
	this->positionFloat = dx::XMFLOAT3(pos.x, pos.y, pos.z);
}

void Camera::setWorldMatrix()
{
	this->worldSpace = dx::XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z) * 
		dx::XMMatrixRotationRollPitchYaw(this->rotationFloat.x, this->rotationFloat.y, this->rotationFloat.z) * 
		dx::XMMatrixTranslation(this->positionFloat.x, this->positionFloat.y, this->positionFloat.z);
}

dx::XMMATRIX Camera::getWorldMatrix() const
{
	return this->worldSpace;
}

dx::XMVECTOR Camera::getPosition() const
{
	return this->position;
}

void Camera::setPosition(dx::XMFLOAT4 pos)
{
	this->position = dx::XMVectorSet(pos.x,pos.y,pos.z,pos.w);
	this->positionFloat = dx::XMFLOAT3(pos.x, pos.y, pos.z);
}

dx::XMFLOAT3 Camera::getPositionFloat() const
{
	return this->positionFloat;
}

void Camera::setPositionFloat(dx::XMFLOAT3 pos)
{
	this->positionFloat = pos;
}

dx::XMVECTOR Camera::getTarget() const
{
	return this->target;
}

void Camera::setTarget(dx::XMFLOAT4 target)
{
	this->target = dx::XMVectorSet(target.x, target.y, target.z, target.w);
}

dx::XMVECTOR Camera::getUp() const
{
	return this->up;
}

void Camera::setUp(dx::XMFLOAT4 up)
{
	this->up = dx::XMVectorSet(up.x, up.y, up.z, up.w);
}

dx::XMVECTOR Camera::getRotation() const
{
	return this->rotation;
}

void Camera::setRotation(dx::XMFLOAT4 rot)
{
	rotation = dx::XMVectorSet(rot.x,rot.y,rot.z,rot.w);
	this->rotationFloat = dx::XMFLOAT3(rot.x, rot.y, rot.z);
}

dx::XMMATRIX Camera::getViewMatrix() const
{
	return this->viewMatrix;
}

void Camera::setViewMatrix(dx::XMVECTOR pos, dx::XMVECTOR target, dx::XMVECTOR up)
{
	this->viewMatrix = dx::XMMatrixLookAtLH(pos,target,up); // creates the view space matrix
}

void Camera::setViewMatrix()
{
	this->viewMatrix = dx::XMMatrixLookAtLH(position, target, defaultUpVector); // creates the view space matrix
}

float Camera::getFieldOfView() const
{
	return this->fieldOfView;
}

void Camera::setFieldOfView(float fov)
{
	float fieldOfViewD = fov;
	fieldOfView = (fieldOfViewD / 360.0f)*dx::XM_2PI;
}

float Camera::getAspectRatio() const
{
	return this->aspectRatio;
}

void Camera::setAspectRatio(float width, float height)
{
	aspectRatio = width / height;
}

float Camera::getNearZ() const
{
	return this->nearZ;
}

void Camera::setNearZ(float nearZ)
{
	this->nearZ = nearZ;
}

float Camera::getFarZ() const
{
	return this->farZ;
}

void Camera::setFarZ(float farZ)
{
	this->farZ = farZ;
}

float Camera::getPitch() const
{
	return this->pitch;
}

void Camera::setPitch(float pitch)
{
	this->pitch = pitch;
}

float Camera::getYaw() const
{
	return this->yaw;
}

void Camera::setYaw(float yaw)
{
	this->yaw = yaw;
}

float Camera::getOffset() const
{
	return this->cameraOffset;
}

void Camera::setOffset(float offset)
{
	this->cameraOffset = offset;
}


dx::XMMATRIX Camera::getProjectionMatrix() const
{
	return this->projectionMatrix;
}

void Camera::setProjectionMatrix(float fieldOfView, float aspectRatio, float nearZ, float farZ)
{
	projectionMatrix = dx::XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearZ, farZ); // creates the projection space matrix
}

void Camera::setProjectionMatrix()
{
	projectionMatrix = dx::XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearZ, farZ); // creates the projection space matrix
}

dx::XMMATRIX Camera::getOrthographicMatrix() const
{
	return this->orthographicMatrix;
}

void Camera::setOrthographicMatrix(float width, float height, float nearZ, float farZ)
{
	orthographicMatrix = dx::XMMatrixOrthographicLH(width, height, nearZ, farZ);
}

void Camera::setEverythingProjection(float windowWidth, float windowHeight, float width, float height, float nearPlane, float farPlane, float fieldOfView)
{
	setAspectRatio(windowWidth, windowHeight);
	setNearZ(nearPlane);
	setFarZ(farPlane);
	setFieldOfView(fieldOfView);
	setProjectionMatrix();
	setViewMatrix();
	setOrthographicMatrix(width, height, this->nearZ, this->farZ);
}

dx::XMFLOAT3 Camera::cameraToTargetVector()
{
	dx::XMFLOAT3 directionVector;
	directionVector.x = dx::XMVectorGetX(target) - dx::XMVectorGetX(position);
	directionVector.y = dx::XMVectorGetY(target) - dx::XMVectorGetY(position);
	directionVector.z = dx::XMVectorGetZ(target) - dx::XMVectorGetZ(position);
	return directionVector;
}

dx::XMFLOAT4X4 Camera::getProjectionMatrix4X4() 
{
	dx::XMStoreFloat4x4(&projectionMatrix4X4, projectionMatrix);
	return this->projectionMatrix4X4;
}

dx::XMFLOAT4X4 Camera::getviewMatrix4X4()
{
	dx::XMStoreFloat4x4(&viewMatrix4X4, viewMatrix);
	return this->viewMatrix4X4;
}

void Camera::update()
{
	rotationMatrix = dx::XMMatrixRotationRollPitchYaw(pitch, yaw, 0);
	target = dx::XMVector3TransformCoord(defaultForwardVector, rotationMatrix);
	target = dx::XMVector3Normalize(target);

	dx::XMMATRIX rotateYTempMatrix;
	rotateYTempMatrix = dx::XMMatrixRotationY(yaw);

	rightVector = dx::XMVector3TransformCoord(defaultRightVector, rotateYTempMatrix);
	up = dx::XMVector3TransformCoord(up, rotateYTempMatrix);
	forwardVector = dx::XMVector3TransformCoord(defaultForwardVector, rotateYTempMatrix);

	position = dx::XMVectorAdd(position, dx::XMVectorScale(rightVector, moveLeftRight));
	position = dx::XMVectorAdd(position, dx::XMVectorScale(forwardVector, moveBackForward));
	
	moveLeftRight = 0;
	moveBackForward = 0;
	
	target = dx::XMVectorAdd(position, target);
	viewMatrix = dx::XMMatrixLookAtLH(position, target, up);
}

void Camera::updateProjectionMatrix()
{
	projectionMatrix = dx::XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearZ, farZ); // creates the projection space matrix
}

void Camera::move(dx::XMFLOAT4 pos)
{
	dx::XMFLOAT4 temp;
	dx::XMStoreFloat4(&temp, position);
	temp.x = temp.x + pos.x;
	temp.y = temp.y + pos.y;
	temp.z = temp.z + pos.z;
	temp.w = temp.w + pos.w;

	position = dx::XMVectorSet(temp.x, temp.y, temp.z, temp.w);
	positionFloat = dx::XMFLOAT3(temp.x, temp.y, temp.z);
}

void Camera::changeRotation(dx::XMFLOAT4 rot)
{
	dx::XMFLOAT4 temp;
	dx::XMStoreFloat4(&temp, rotation);
	temp.x = temp.x + rot.x;
	temp.y = temp.y + rot.y;
	temp.z = temp.z + rot.z;
	temp.w = temp.w + rot.w;

	rotation = dx::XMVectorSet(temp.x, temp.y, temp.z, temp.w);
}

bool Camera::getCollisionBool() const
{
	return this->collision;
}

void Camera::setCollisionBool(float collision)
{
	this->collision = collision;
}
