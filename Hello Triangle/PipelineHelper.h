#pragma once

#include "MyIncludes.h"
#include <d3dcompiler.h>
#include <string>
#include <iostream>

void createInputLayout(ID3D11Device*& devicePtr, ID3D11InputLayout*& inputLayoutPtr, ID3DBlob *& vsBlobPtr, ID3DBlob *& psBlobPtr, ID3DBlob *& errorBlob, HRESULT hr);

void setupRasterizer(ID3D11Device *& devicePtr, ID3D11DeviceContext *& deviceContextPtr, ID3D11RasterizerState *& rasterizerStatePtr, HRESULT hr);

void createSamplerState(ID3D11Device *&devicePtr, ID3D11SamplerState *&samplerStatePtr, HRESULT hr);
void createSamplerStateClapp(ID3D11Device *&devicePtr, ID3D11SamplerState *&samplerStatePtr, HRESULT hr);

void compileVertexShader(ID3D11Device *& devicePtr, ID3D11VertexShader *&vertexShaderPtr, std::wstring fileName, std::string entryPoint, std::string target, ID3DBlob *& vsBlobPtr, ID3DBlob *& errorBlob, UINT flags, HRESULT hr);
void compilePixelShader(ID3D11Device *& devicePtr, ID3D11PixelShader *&pixelShaderPtr, std::wstring fileName, std::string entryPoint, std::string target, ID3DBlob *& psBlobPtr, ID3DBlob *& errorBlob, UINT flags, HRESULT hr);

void compileGeometryShader(ID3D11Device *& devicePtr, ID3D11GeometryShader *&geometryShaderPtr, std::wstring fileName, std::string entryPoint, std::string target, ID3DBlob *& gsBlobPtr, ID3DBlob *& errorBlob, UINT flags, HRESULT hr);

void createVertexShader(ID3D11Device*& devicePtr, ID3D11VertexShader *&vertexShaderPtr, ID3DBlob *& vsBlobPtr, HRESULT hr);
void createPixelShader(ID3D11Device*& devicePtr, ID3D11PixelShader *&pixelShaderPtr, ID3DBlob *& psBlobPtr, HRESULT hr);
void createGeometryShader(ID3D11Device*& devicePtr, ID3D11GeometryShader *&geometryShaderShaderPtr, ID3DBlob *& gsBlobPtr, HRESULT hr);

void createConstantBuffer(ID3D11Device *& devicePtr, ID3D11DeviceContext *& deviceContextPtr, ConstantBuffer &cBufferPerObject, ID3D11Buffer *&constantBufferPerObjectPtr);

void setupPipeline(ID3D11Device*& devicePtr, ID3D11DeviceContext *& deviceContextPtr, ID3D11RasterizerState *& rasterizerStatePtr, ID3D11SamplerState *& samplerStatePtr,ID3D11InputLayout*& inputLayoutPtr, ID3DBlob *& vsBlobPtr, ID3DBlob *& psBlobPtr, ID3DBlob *& errorBlob, HRESULT hr);

