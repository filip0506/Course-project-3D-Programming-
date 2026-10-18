
#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h> // win32
#include <d3d11.h> // D3D 11
#include <DirectXMath.h>
#include <assert.h> // for assert()

namespace dx = DirectX;

static const dx::XMFLOAT3 up = dx::XMFLOAT3(0, 1, 0);
static const dx::XMFLOAT3 down = dx::XMFLOAT3(0, -1, 0);
static const dx::XMFLOAT3 right = dx::XMFLOAT3(1, 0, 0);
static const dx::XMFLOAT3 left = dx::XMFLOAT3(-1, 0, 0);
static const dx::XMFLOAT3 forward = dx::XMFLOAT3(0, 0, 1);
static const dx::XMFLOAT3 backward = dx::XMFLOAT3(0, 0, -1);


struct ConstantBuffer
{
	dx::XMMATRIX WVP;
	dx::XMMATRIX world;
	dx::XMMATRIX lightViewMatrix = dx::XMMatrixIdentity();
	dx::XMMATRIX lightProjectionMatrix = dx::XMMatrixIdentity();
	dx::XMFLOAT3 cameraPos;
	int hasANormalMap = FALSE;
	dx::XMFLOAT4 animationSpeed = dx::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
};


struct MaterialBuffer
{
	dx::XMFLOAT3 kd; // diffuse color
	float padding;
	dx::XMFLOAT3 ka; // ambient color
	float padding1;
	dx::XMFLOAT3 ks; // specular color
	float ns; // specular exponent
};

struct LightBuffer {
	dx::XMFLOAT3 position;
	float pad;
	dx::XMFLOAT3 ambientColor;
	float pad1;
	dx::XMFLOAT4 diffuse;
	dx::XMFLOAT3 direction;
	float pad2;
	dx::XMFLOAT3 attenuation;
	float globalAmbient;
};

struct GeometryShaderBuffer
{
	dx::XMFLOAT3 cameraPos;
	float padding;
};

