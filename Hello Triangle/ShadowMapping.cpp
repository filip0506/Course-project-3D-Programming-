#include "ShadowMapping.h"

ShadowMapping::ShadowMapping(ID3D11Device * devicePtr, int width, int height)
{
	this->width = width;
	this->height = height;

	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL |	D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ID3D11Texture2D* depthMap = {};
	hr = devicePtr->CreateTexture2D(&texDesc, 0, &depthMap);
	assert(SUCCEEDED(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
    hr = devicePtr->CreateDepthStencilView(depthMap, &dsvDesc, &depthMapDepthStencilViewPtr);
	assert(SUCCEEDED(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	hr = devicePtr->CreateShaderResourceView(depthMap, &srvDesc, &depthMapShaderResourceViewPtr);

	depthMap->Release();
}

void ShadowMapping::BindDsvAndSetNullRenderTarget(ID3D11DeviceContext * deviceContextPtr)
{
	ID3D11RenderTargetView *renderTargets = nullptr;
	deviceContextPtr->OMSetRenderTargets(1, &renderTargets, depthMapDepthStencilViewPtr);
	deviceContextPtr->ClearDepthStencilView(depthMapDepthStencilViewPtr, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView *& ShadowMapping::getDepthMapShaderResourceView()
{
	return this->depthMapShaderResourceViewPtr;
}

void ShadowMapping::release()
{
	if (depthMapShaderResourceViewPtr != nullptr)
	{
		depthMapShaderResourceViewPtr->Release();
		depthMapShaderResourceViewPtr = nullptr;
	}
	if (depthMapDepthStencilViewPtr != nullptr)
	{
		depthMapDepthStencilViewPtr->Release();
		depthMapDepthStencilViewPtr = nullptr;
	}
}
