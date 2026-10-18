#include "D3D11Helper.h"

void createInterfaces(ID3D11Device *& devicePtr, ID3D11DeviceContext *& deviceContextPtr, IDXGISwapChain *& swapChainPtr, UINT windowWidth, UINT windowHeight, HWND hwnd, HRESULT hr, UINT flags)
{
	DXGI_SWAP_CHAIN_DESC swapChainDescr = { 0 }; //Describes a swap chain.
	swapChainDescr.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDescr.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDescr.BufferDesc.Width = windowWidth;
	swapChainDescr.BufferDesc.Height = windowHeight;
	swapChainDescr.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; //32-bit unsigned-normalized-integer format that supports 8 bits for each color channel and 8-bit alpha.
	swapChainDescr.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDescr.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDescr.SampleDesc.Count = 1;
	swapChainDescr.SampleDesc.Quality = 0;
	swapChainDescr.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDescr.BufferCount = 1;
	swapChainDescr.OutputWindow = hwnd;
	swapChainDescr.Windowed = true;
	swapChainDescr.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDescr.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D_FEATURE_LEVEL feature_Level;
	
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		flags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDescr,
		&swapChainPtr,
		&devicePtr,
		&feature_Level,
		&deviceContextPtr);
	assert(S_OK == hr && swapChainPtr && devicePtr && deviceContextPtr);
}

void createRenderTargetView(ID3D11Device *&devicePtr, IDXGISwapChain *&swapChainPtr, ID3D11RenderTargetView *&renderTargetViewPtr, HRESULT hr)
{
	ID3D11Texture2D* framebuffer;
	//back buffer
	hr = swapChainPtr->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&framebuffer);
	assert(SUCCEEDED(hr));

	//render target
	hr = devicePtr->CreateRenderTargetView(framebuffer, 0, &renderTargetViewPtr);
	assert(SUCCEEDED(hr));
	framebuffer->Release();
}

void createDepthStencil(ID3D11Device *& devicePtr, UINT width, UINT height, ID3D11Texture2D *& depthStencilTexture, ID3D11DepthStencilView *& depthStencilViewPtr, HRESULT hr)
{
	D3D11_TEXTURE2D_DESC depthDescTex = {};
	depthDescTex.Width = width;
	depthDescTex.Height = height;
	depthDescTex.MipLevels = 1;
	depthDescTex.ArraySize = 1;
	depthDescTex.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	depthDescTex.SampleDesc.Count = 1;
	depthDescTex.SampleDesc.Quality = 0;
	depthDescTex.Usage = D3D11_USAGE_DEFAULT;
	depthDescTex.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDescTex.CPUAccessFlags = 0;
	depthDescTex.MiscFlags = 0;
	hr = devicePtr->CreateTexture2D(&depthDescTex, nullptr, &depthStencilTexture);
	assert(SUCCEEDED(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthDescView = {};
	depthDescView.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	depthDescView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthDescView.Texture2D.MipSlice = 0;

	hr = devicePtr->CreateDepthStencilView(depthStencilTexture, &depthDescView, &depthStencilViewPtr);
	assert(SUCCEEDED(hr));
}


void setViewport(ID3D11DeviceContext *&deviceContextPtr, D3D11_VIEWPORT &viewPort, UINT windowWidth, UINT windowHeight, HWND hwnd)
{
	RECT winRect;
	GetClientRect(hwnd, &winRect);

	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (FLOAT)(winRect.right - winRect.left);
	viewPort.Height = (FLOAT)(winRect.bottom - winRect.top);
	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;
	deviceContextPtr->RSSetViewports(1, &viewPort);
}

void drawFullScreenQuad(Mesh & mesh, ID3D11DeviceContext *&deviceContextPtr)
{
	deviceContextPtr->IASetVertexBuffers(0, 1, &mesh.vertexBufferPtr, &mesh.getVertexStride(), &mesh.getVertexOffset());
	deviceContextPtr->Draw(mesh.getVertexCount(), 0);
}

void drawDeferred(Mesh & mesh, Camera & camera, Light & light, ID3D11DeviceContext *& deviceContextPtr, ID3D11VertexShader *& vertexShaderPtr, ID3D11PixelShader *& pixelShaderPtr, ID3D11SamplerState *& samplerStatePtr, ID3D11Buffer *& constantBufferPerObjectPtr, ConstantBuffer & constantBuffer, ID3D11Buffer *& constantBufferPtrMaterial, MaterialBuffer & material)
{
	//However, if you want to rotate an object around a certain point, then it is scale, point translation, rotation and lastly object translation.
	constantBuffer.world = dx::XMMatrixTranspose(mesh.getWorldMatrix());
	constantBuffer.WVP = dx::XMMatrixTranspose((mesh.getWorldMatrix() * camera.getViewMatrix() *  camera.getProjectionMatrix()));
	constantBuffer.lightViewMatrix = dx::XMMatrixTranspose(light.getCamera().getViewMatrix());
	constantBuffer.lightProjectionMatrix = dx::XMMatrixTranspose(light.getCamera().getOrthographicMatrix());
	constantBuffer.cameraPos = camera.getPositionFloat();
	constantBuffer.hasANormalMap = mesh.hasNormalMap();
	constantBuffer.animationSpeed = dx::XMFLOAT4(0, 0, 0, 0);

	deviceContextPtr->VSSetConstantBuffers(0, 1, &constantBufferPerObjectPtr);
	deviceContextPtr->PSSetConstantBuffers(0, 1, &constantBufferPerObjectPtr);
	deviceContextPtr->UpdateSubresource(constantBufferPerObjectPtr, 0, nullptr, &constantBuffer, 0, 0);

	//Material
	deviceContextPtr->PSSetConstantBuffers(1, 1, &constantBufferPtrMaterial);
	deviceContextPtr->UpdateSubresource(constantBufferPtrMaterial, 0, nullptr, &material, 0, 0);

	deviceContextPtr->IASetVertexBuffers(0, 1, &mesh.vertexBufferPtr, &mesh.getVertexStride(), &mesh.getVertexOffset());

	deviceContextPtr->PSSetShaderResources(0, 1, &mesh.textureResourceViewPtr);
	deviceContextPtr->PSSetShaderResources(1, 1, &mesh.normalTextureResourceViewPtr);
	deviceContextPtr->PSSetSamplers(0, 1, &samplerStatePtr);

	deviceContextPtr->Draw(mesh.getVertexCount(), 0);
}

void drawShadow(Mesh & mesh, Camera & camera, Light & light, ID3D11DeviceContext *& deviceContextPtr, ID3D11VertexShader *& vertexShaderPtr, ID3D11Buffer *& constantBufferPerObjectPtr, ConstantBuffer & constantBuffer)
{
	constantBuffer.world = dx::XMMatrixTranspose(mesh.getWorldMatrix());
	constantBuffer.WVP = dx::XMMatrixTranspose((mesh.getWorldMatrix() * light.getCamera().getViewMatrix() *  light.getCamera().getOrthographicMatrix()));
	constantBuffer.lightViewMatrix = dx::XMMatrixTranspose(light.getCamera().getViewMatrix());
	constantBuffer.lightProjectionMatrix = dx::XMMatrixTranspose(light.getCamera().getOrthographicMatrix());
	constantBuffer.cameraPos = camera.getPositionFloat();
	constantBuffer.hasANormalMap = mesh.hasNormalMap();
	constantBuffer.animationSpeed = dx::XMFLOAT4(0, 0, 0, 0);

	deviceContextPtr->VSSetConstantBuffers(0, 1, &constantBufferPerObjectPtr);
	deviceContextPtr->UpdateSubresource(constantBufferPerObjectPtr, 0, nullptr, &constantBuffer, 0, 0);
	deviceContextPtr->VSSetShader(vertexShaderPtr, nullptr, 0);
	deviceContextPtr->PSSetShader(nullptr, nullptr, 0);
	deviceContextPtr->IASetVertexBuffers(0, 1, &mesh.vertexBufferPtr, &mesh.getVertexStride(), &mesh.getVertexOffset());
	deviceContextPtr->Draw(mesh.getVertexCount(), 0);
}

void setupD3D11(UINT windowWidth, UINT windowHeight, HWND hwnd, ID3D11Device *& devicePtr, ID3D11DeviceContext *& deviceContextPtr, IDXGISwapChain *& swapChainPtr, ID3D11RenderTargetView *& renderTargetViewPtr, ID3D11Texture2D *& depthStencilTexture, ID3D11DepthStencilView *& depthStencilViewPtr, D3D11_VIEWPORT & viewPort, HRESULT hr, UINT flags)
{
	createInterfaces(devicePtr, deviceContextPtr, swapChainPtr, windowWidth, windowHeight, hwnd, hr, flags);
	createRenderTargetView(devicePtr, swapChainPtr, renderTargetViewPtr, hr);
	createDepthStencil(devicePtr, windowWidth, windowHeight, depthStencilTexture, depthStencilViewPtr, hr);
	setViewport(deviceContextPtr, viewPort, windowWidth, windowHeight, hwnd);
}
