#include "DeferredRender.h"

DeferredRender::DeferredRender()
{
}

DeferredRender::DeferredRender(int width, int height)
{
	this->width = width;
	this->height = height;
}

void DeferredRender::Initialize(ID3D11Device *devicePtr)
{
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	for (int i = 0; i < bufferCount; i++)
	{
		hr = devicePtr->CreateTexture2D(&textureDesc, nullptr, &renderTargetTextureArray[i]);
		assert(SUCCEEDED(hr));
	}

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetDesc = {};
	renderTargetDesc.Format = textureDesc.Format;
	renderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetDesc.Texture2D.MipSlice = 0;

	for (int i = 0; i < bufferCount; i++)
	{
		hr = devicePtr->CreateRenderTargetView(renderTargetTextureArray[i], &renderTargetDesc, &renderTargetArray[i]);
		assert(SUCCEEDED(hr));
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc = {};
	shaderResourceDesc.Format = textureDesc.Format;
	shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceDesc.Texture2D.MipLevels = 1;

	for (int i = 0; i < bufferCount; i++)
	{
		hr = devicePtr->CreateShaderResourceView(renderTargetTextureArray[i], &shaderResourceDesc, &shaderResourceViewArray[i]);
		assert(SUCCEEDED(hr));
	}
}

void DeferredRender::release()
{
	for (int i = 0; i < bufferCount; i++)
	{
		if (shaderResourceViewArray[i] != nullptr)
		{
			shaderResourceViewArray[i]->Release();
			shaderResourceViewArray[i] = nullptr;
		}

		if (renderTargetArray[i] != nullptr)
		{
			renderTargetArray[i]->Release();
			renderTargetArray[i] = nullptr;
		}

		if (renderTargetTextureArray[i] != nullptr)
		{
			renderTargetTextureArray[i]->Release();
			renderTargetTextureArray[i] = nullptr;
		}
	}
}

void DeferredRender::setRenderTargets(ID3D11DeviceContext * deviceContextPtr, ID3D11DepthStencilView * depthStencilViewPtr)
{
	deviceContextPtr->OMSetRenderTargets(bufferCount, renderTargetArray, depthStencilViewPtr);
}

void DeferredRender::setRenderTargets(ID3D11DeviceContext * deviceContextPtr, ID3D11RenderTargetView * render_Target_View_Array_Ptr[], ID3D11DepthStencilView * depthStencilViewPtr)
{
	deviceContextPtr->OMSetRenderTargets(bufferCount, render_Target_View_Array_Ptr, depthStencilViewPtr);
}

void DeferredRender::clearRenderTargets(ID3D11DeviceContext * deviceContextPtr, float backGroundColor[4])
{
	for (int i = 0; i < bufferCount; i++)
	{
		deviceContextPtr->ClearRenderTargetView(renderTargetArray[i], backGroundColor);
	}
}

ID3D11ShaderResourceView *& DeferredRender::getShaderResourceView(int index)
{
	return shaderResourceViewArray[index];
}

