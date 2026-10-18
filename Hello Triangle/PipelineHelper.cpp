#include "PipelineHelper.h"

void createInputLayout(ID3D11Device *& devicePtr, ID3D11InputLayout *& inputLayoutPtr, ID3DBlob *& vsBlobPtr, ID3DBlob *& psBlobPtr, ID3DBlob *& errorBlob, HRESULT hr)
{
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
		//{"COLOR",  0, DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TEXTURE",  0, DXGI_FORMAT_R32G32_FLOAT,   0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		//{"BINORMAL",  0, DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};

	hr = devicePtr->CreateInputLayout(
		inputElementDesc,
		ARRAYSIZE(inputElementDesc),
		vsBlobPtr->GetBufferPointer(),
		vsBlobPtr->GetBufferSize(),
		&inputLayoutPtr);
	assert(SUCCEEDED(hr));
}

void setupRasterizer(ID3D11Device *& devicePtr, ID3D11DeviceContext *& deviceContextPtr, ID3D11RasterizerState *& rasterizerStatePtr, HRESULT hr)
{
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME 

	//rasterizerDesc.CullMode = D3D11_CULL_BACK;  //D3D11_CULL_BACK
	rasterizerDesc.CullMode = D3D11_CULL_NONE;  //D3D11_CULL_BACK

	rasterizerDesc.FrontCounterClockwise = FALSE;

	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;

	hr = devicePtr->CreateRasterizerState(&rasterizerDesc, &rasterizerStatePtr);
	assert(SUCCEEDED(hr));
}

void createSamplerState(ID3D11Device *& devicePtr, ID3D11SamplerState *& samplerStatePtr, HRESULT hr)
{
	D3D11_SAMPLER_DESC samplerStateDesc = {};
	samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; //Tile the texture at every (u,v) integer junction. For example, for u values between 0 and 3, the texture is repeated three times.
	samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerStateDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;// A function that compares sampled data against existing sampled data.  //Never pass the comparison.
	samplerStateDesc.MinLOD = 0;
	samplerStateDesc.MaxLOD = D3D11_FLOAT32_MAX; // no limit

	hr = devicePtr->CreateSamplerState(&samplerStateDesc, &samplerStatePtr);
	assert(SUCCEEDED(hr));
	
}

void createSamplerStateClapp(ID3D11Device *& devicePtr, ID3D11SamplerState *& samplerStatePtr, HRESULT hr)
{
	float color[4] = { 1,1,1,0};
	
	D3D11_SAMPLER_DESC samplerStateDesc = {};
	samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER; //Tile the texture at every (u,v) integer junction. For example, for u values between 0 and 3, the texture is repeated three times.
	samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;

	for (int i = 0; i < 4; i++)
	{
		samplerStateDesc.BorderColor[i] = color[i];
	}

	samplerStateDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;// A function that compares sampled data against existing sampled data.  //Never pass the comparison.
	samplerStateDesc.MinLOD = 0;
	samplerStateDesc.MaxLOD = D3D11_FLOAT32_MAX; // no limit

	hr = devicePtr->CreateSamplerState(&samplerStateDesc, &samplerStatePtr);
	assert(SUCCEEDED(hr));
}

void compileVertexShader(ID3D11Device *& devicePtr, ID3D11VertexShader *& vertexShaderPtr, std::wstring fileName, std::string entryPoint, std::string target, ID3DBlob *& vsBlobPtr, ID3DBlob *& errorBlob, UINT flags, HRESULT hr)
{
	// COMPILE VERTEX SHADER
	hr = D3DCompileFromFile(
		fileName.data(),
		nullptr,
		nullptr,
		entryPoint.c_str(),
		target.c_str(),
		flags,
		0,
		&vsBlobPtr,
		&errorBlob);

	assert(SUCCEEDED(hr));

	if (FAILED(hr)) {
		if (errorBlob) {
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		if (vsBlobPtr) { vsBlobPtr->Release(); }
		assert(false);
	}

	createVertexShader(devicePtr, vertexShaderPtr, vsBlobPtr, hr);
}

void compilePixelShader(ID3D11Device *& devicePtr, ID3D11PixelShader *& pixelShaderPtr, std::wstring fileName, std::string entryPoint, std::string target, ID3DBlob *& psBlobPtr, ID3DBlob *& errorBlob, UINT flags, HRESULT hr)
{
	// COMPILE PIXEL SHADER
	hr = D3DCompileFromFile(
		fileName.data(),
		nullptr,
		nullptr,
		entryPoint.c_str(),
		target.c_str(),
		flags,
		0,
		&psBlobPtr,
		&errorBlob);

	assert(SUCCEEDED(hr));

	if (FAILED(hr)) {
		if (errorBlob) {
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		if (psBlobPtr) { psBlobPtr->Release(); }
		assert(false);
	}
	createPixelShader(devicePtr, pixelShaderPtr, psBlobPtr, hr);
}

void compileGeometryShader(ID3D11Device *& devicePtr, ID3D11GeometryShader *& geometryShaderPtr, std::wstring fileName, std::string entryPoint, std::string target, ID3DBlob *& gsBlobPtr, ID3DBlob *& errorBlob, UINT flags, HRESULT hr)
{
	hr = D3DCompileFromFile(
		fileName.data(),
		nullptr,
		nullptr,
		entryPoint.c_str(),
		target.c_str(),
		flags,
		0,
		&gsBlobPtr,
		&errorBlob);
	if (FAILED(hr)) {
		if (errorBlob) {
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		if (gsBlobPtr) { gsBlobPtr->Release(); }
		assert(false);
	}
	createGeometryShader(devicePtr, geometryShaderPtr, gsBlobPtr, hr);
}

void createVertexShader(ID3D11Device *& devicePtr, ID3D11VertexShader *& vertexShaderPtr, ID3DBlob *& vsBlobPtr, HRESULT hr)
{
	hr = devicePtr->CreateVertexShader(
		vsBlobPtr->GetBufferPointer(),
		vsBlobPtr->GetBufferSize(),
		nullptr,
		&vertexShaderPtr);
	assert(SUCCEEDED(hr));
}

void createPixelShader(ID3D11Device *& devicePtr, ID3D11PixelShader *& pixelShaderPtr, ID3DBlob *& psBlobPtr, HRESULT hr)
{
	hr = devicePtr->CreatePixelShader(
		psBlobPtr->GetBufferPointer(),
		psBlobPtr->GetBufferSize(),
		nullptr,
		&pixelShaderPtr);
	assert(SUCCEEDED(hr));
}
void createGeometryShader(ID3D11Device *& devicePtr, ID3D11GeometryShader *& geometryShaderShaderPtr, ID3DBlob *& gsBlobPtr, HRESULT hr)
{
	hr = devicePtr->CreateGeometryShader(
		gsBlobPtr->GetBufferPointer(),
		gsBlobPtr->GetBufferSize(),
		nullptr,
		&geometryShaderShaderPtr);
	assert(SUCCEEDED(hr));
}

void createConstantBuffer(ID3D11Device *& devicePtr, ID3D11DeviceContext *& deviceContextPtr, ConstantBuffer &cBufferPerObject, ID3D11Buffer *&constantBufferPerObjectPtr)
{
	D3D11_BUFFER_DESC constantBuffDescr = {};
	constantBuffDescr.ByteWidth = sizeof(cBufferPerObject);
	constantBuffDescr.Usage = D3D11_USAGE_DEFAULT;
	constantBuffDescr.BindFlags = D3D11_BIND_CONSTANT_BUFFER; //binding a buffer of type to the BindFlags

	D3D11_SUBRESOURCE_DATA csdData = { 0 }; // points to our array of floats
	csdData.pSysMem = &cBufferPerObject;
	HRESULT hr = devicePtr->CreateBuffer(&constantBuffDescr, &csdData, &constantBufferPerObjectPtr);
	assert(SUCCEEDED(hr));//gives us a pointer to a vertex buffer.
}

void setupPipeline(ID3D11Device *& devicePtr, ID3D11DeviceContext *& deviceContextPtr, ID3D11RasterizerState *& rasterizerStatePtr, ID3D11SamplerState *& samplerStatePtr, ID3D11InputLayout *& inputLayoutPtr, ID3DBlob *& vsBlobPtr, ID3DBlob *& psBlobPtr, ID3DBlob *& errorBlob, HRESULT hr)
{
	createInputLayout(devicePtr, inputLayoutPtr, vsBlobPtr, psBlobPtr, errorBlob, hr);
	setupRasterizer(devicePtr, deviceContextPtr, rasterizerStatePtr, hr);
	createSamplerState(devicePtr, samplerStatePtr, hr);
}
