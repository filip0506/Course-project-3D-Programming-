#pragma once
#include "MyIncludes.h"
#include "Mesh.h"
#include "Camera.h"
#include "Light.h"
#include <iostream>
#include <vector>
#include "Terrain.h"



void createInterfaces(ID3D11Device*& devicePtr, ID3D11DeviceContext*& deviceContextPtr, IDXGISwapChain*& swapChainPtr, UINT windowWidth, UINT windowHeight, HWND hwnd, HRESULT hr, UINT flags);

void createRenderTargetView(ID3D11Device *&devicePtr, IDXGISwapChain *&swapChainPtr, ID3D11RenderTargetView *&renderTargetViewPtr, HRESULT hr);

void createDepthStencil(ID3D11Device *&devicePtr, UINT width, UINT height, ID3D11Texture2D *&depthStencilTexture, ID3D11DepthStencilView *&depthStencilViewPtr, HRESULT hr);

void setViewport(ID3D11DeviceContext *&deviceContextPtr,D3D11_VIEWPORT &viewPort, UINT windowWidth, UINT windowHeight, HWND hwnd);

void drawFullScreenQuad(Mesh & mesh, ID3D11DeviceContext *&deviceContextPtr);

void drawDeferred(Mesh & mesh, Camera & camera, Light & light, ID3D11DeviceContext *& deviceContextPtr, ID3D11VertexShader *&vertexShaderPtr, ID3D11PixelShader *&pixelShaderPtr, ID3D11SamplerState *& samplerStatePtr, ID3D11Buffer *&constantBufferPerObjectPtr, ConstantBuffer &constantBuffer, ID3D11Buffer *& constantBufferPtrMaterial, MaterialBuffer & material);

void drawShadow(Mesh & mesh, Camera & camera, Light & light, ID3D11DeviceContext *& deviceContextPtr, ID3D11VertexShader *&vertexShaderPtr, ID3D11Buffer *&constantBufferPerObjectPtr, ConstantBuffer &constantBuffer);

void setupD3D11(UINT windowWidth, UINT windowHeight, HWND hwnd, ID3D11Device*& devicePtr,
	ID3D11DeviceContext*& deviceContextPtr, IDXGISwapChain*& swapChainPtr, ID3D11RenderTargetView*& renderTargetViewPtr,
	ID3D11Texture2D*& depthStencilTexture, ID3D11DepthStencilView*& depthStencilViewPtr,D3D11_VIEWPORT& viewPort, HRESULT hr, UINT flags);

