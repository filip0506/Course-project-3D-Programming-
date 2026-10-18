#pragma once
#include "MyIncludes.h"
class ShadowMapping
{
private:

	int width = 0;
	int height = 0;

	ID3D11ShaderResourceView* depthMapShaderResourceViewPtr = nullptr;
	ID3D11DepthStencilView* depthMapDepthStencilViewPtr = nullptr;
	
	HRESULT hr;

public:

	ShadowMapping(ID3D11Device* devicePtr, int width, int height);

	void BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* deviceContextPtr);
	ID3D11ShaderResourceView *&getDepthMapShaderResourceView();
	void release();
};



