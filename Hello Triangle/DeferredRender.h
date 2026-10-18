#pragma once
#include "MyIncludes.h"

class DeferredRender
{
private:

	static const int bufferCount = 3;
	int width;
	int height;

	ID3D11Texture2D *renderTargetTextureArray[bufferCount] = {};
	ID3D11RenderTargetView *renderTargetArray[bufferCount] = {};
	ID3D11ShaderResourceView *shaderResourceViewArray[bufferCount] = {};

	HRESULT hr;

public:

	DeferredRender();
	DeferredRender(int width, int height);

	void Initialize(ID3D11Device *devicePtr);
	void release();

	void setRenderTargets(ID3D11DeviceContext *deviceContextPtr, ID3D11DepthStencilView* depthStencilViewPtr);
	void setRenderTargets(ID3D11DeviceContext *deviceContextPtr, ID3D11RenderTargetView* render_Target_View_Array_Ptr[], ID3D11DepthStencilView* depthStencilViewPtr);

	void clearRenderTargets(ID3D11DeviceContext *deviceContextPtr,float backGroundColor[4]);
	
	ID3D11ShaderResourceView *& getShaderResourceView(int index);

};
