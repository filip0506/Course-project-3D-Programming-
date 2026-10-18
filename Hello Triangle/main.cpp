#ifndef UNICODE
#define UNICODE
#endif 

// Author: Filip Michael
// 3D PROGRAMMING, DV1568: PROJECT


// headers
#include "MyIncludes.h"

#include <dxgi.h> // graphics devices and swap chain
#include <stdint.h>
#include <d3dcompiler.h>
#include <vector>
#include <WICTextureLoader.h>
#include "Vertex.h"
#include "Camera.h"
#include "Light.h"
#include <string>
#include <iostream>
#include "Timer.h"
#include "OBJParser.h"
#include "DDSTextureLoader.h"
#include "Mesh.h"
#include "D3D11Helper.h"
#include "PipelineHelper.h"
#include "Input.h"
#include "Terrain.h"
#include "DeferredRender.h"
#include "ShadowMapping.h"
#include "GameManager.h"
#include <wrl/client.h>

// libraries
#pragma comment( lib, "user32" ) // win32 stuff
#pragma comment( lib, "gdi32" ) // graphics device interface
#pragma comment( lib, "d3d11" ) // Direct3D 11
#pragma comment( lib, "d3dcompiler.lib" ) // shader compiler (need this later)
#pragma comment( lib, "DirectXTK.lib")

namespace dx = DirectX;

static const int windowWidth = 640;
static const int windowHeight = 480;

float rotationAngle(float angle);

void printCamera(Camera &camera);

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND InitWindow(HINSTANCE hInstance);
//-------------------------------

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	dx::XMFLOAT4 speed = dx::XMFLOAT4(0,0,0,0);
	float speedvar = 0;

	AllocConsole();
	FILE* freOp;
	freopen_s(&freOp, "CONOUT$", "w", stdout);

	HRESULT hr = CoInitialize(nullptr);
	assert(SUCCEEDED(hr));

	// Direct3D device and swap chain.

	ID3D11Device *devicePtr = nullptr; //Returns the address of a pointer to an ID3D11Device object that represents the device created. If this parameter is NULL, no ID3D11Device will be returned'.
	ID3D11DeviceContext *deviceContextPtr = nullptr; //Returns the address of a pointer to an ID3D11DeviceContext object that represents the device context. If this parameter is NULL, no ID3D11DeviceContext will be returned.
	IDXGISwapChain *swapChainPtr = nullptr; //Returns the address of a pointer to the IDXGISwapChain object that represents the swap chain used for rendering.

	// Render target view for the back buffer of the swap chain.
	ID3D11RenderTargetView *renderTargetViewPtr = nullptr; //Address of a pointer to an ID3D11RenderTargetView. Set this parameter to NULL to validate the other input parameters (the method will return S_FALSE if the other input parameters pass validation).

	ID3D11Texture2D *depthStencilTexture;
	ID3D11DepthStencilView *depthStencilViewPtr;

	ID3D11RasterizerState *rasterizerStatePtr;

	ID3DBlob *vsBlobPtr = nullptr, *psBlobPtr = nullptr, *errorBlob = nullptr;
	ID3D11InputLayout *inputLayoutPtr = nullptr;
	ID3D11VertexShader *vertexShaderPtr = nullptr;
	ID3D11PixelShader *pixelShaderPtr = nullptr;
	ID3D11SamplerState *samplerStatePtr; //is used to specify all of the parameters that are used when a shader program samples a texture resource.
	
	ID3D11DepthStencilState *depthStencileState;

	ID3D11GeometryShader *geometryShaderPtr = nullptr;
	ID3DBlob *gsBlobPtr = nullptr;
	ID3DBlob *gsErrorBlob = nullptr;

	// DeferredRender

	DeferredRender deferredRender(windowWidth,windowHeight);

	ID3D11VertexShader *firstPassVertexShaderPtr = nullptr;
	ID3D11PixelShader *firstPassPixelShaderPtr = nullptr;
	ID3DBlob *vsFirstPassBlobPtr = nullptr;
	ID3DBlob *psFirstPassBlobPtr = nullptr;
	ID3DBlob *errorFirstPassBlob = nullptr;

	ID3D11VertexShader *secondPassVertexShaderPtr = nullptr;
	ID3D11PixelShader *secondPassPixelShaderPtr = nullptr;
	ID3DBlob *vsSecondPassBlobPtr = nullptr;
	ID3DBlob *psSecondPassBlobPtr = nullptr;
	ID3DBlob *errorSecondPassBlob = nullptr;

	// Shadow mapping

	ID3D11VertexShader *shadowPassVertexShaderPtr = nullptr;
	ID3DBlob *vsShadowPassBlobPtr = nullptr;
	ID3DBlob *errorShadowPassBlob = nullptr;

	//----------------
	
	D3D11_VIEWPORT viewPort;

	MSG msg = {};
	bool shouldClose = false;
	HWND hwnd = InitWindow(hInstance);

	Input mouseKeyboardInput(hInstance, hwnd, windowWidth, windowHeight);

	//----------------------------------
	UINT flags = D3D11_CREATE_DEVICE_SINGLETHREADED; // Use this flag if your application will only call methods of Direct3D 11 interfaces from a single thread
	#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif
	//----------------------------------

	setupD3D11(windowWidth, windowHeight, hwnd, devicePtr,deviceContextPtr,swapChainPtr,renderTargetViewPtr,depthStencilTexture, depthStencilViewPtr, viewPort, hr, flags);

	flags = D3DCOMPILE_ENABLE_STRICTNESS; // Forces strict compile, which might not allow for legacy syntax.
	#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG; // add more debug output
	#endif

	compileVertexShader(devicePtr, firstPassVertexShaderPtr, L"FirstPassShader.hlsl", "VS_Main", "vs_5_0", vsFirstPassBlobPtr, errorFirstPassBlob, flags, hr);
	compilePixelShader(devicePtr, firstPassPixelShaderPtr, L"FirstPassShader.hlsl", "PS_Main", "ps_5_0", psFirstPassBlobPtr, errorFirstPassBlob, flags, hr);

	compileVertexShader(devicePtr, secondPassVertexShaderPtr, L"SecondPassShader.hlsl", "VS_Main", "vs_5_0", vsSecondPassBlobPtr, errorSecondPassBlob, flags, hr);
	compilePixelShader(devicePtr, secondPassPixelShaderPtr, L"SecondPassShader.hlsl", "PS_Main", "ps_5_0", psSecondPassBlobPtr, errorSecondPassBlob, flags, hr);

	compileVertexShader(devicePtr, shadowPassVertexShaderPtr, L"ShadowPassShader.hlsl", "VS_Main", "vs_5_0", vsShadowPassBlobPtr, errorShadowPassBlob, flags, hr);

	compileGeometryShader(devicePtr, geometryShaderPtr, L"GeometryShader.hlsl", "main", "gs_5_0", gsBlobPtr, gsErrorBlob, flags, hr);


	// DeferredRender
	deferredRender.Initialize(devicePtr);

	setupPipeline(devicePtr, deviceContextPtr, rasterizerStatePtr, samplerStatePtr, inputLayoutPtr, vsFirstPassBlobPtr, psFirstPassBlobPtr, errorFirstPassBlob, hr);


	//--------------------------------------------------------
	D3D11_DEPTH_STENCIL_DESC depthDesc = {};
	depthDesc.DepthEnable = TRUE;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	hr = devicePtr->CreateDepthStencilState(&depthDesc, &depthStencileState);
	assert(SUCCEEDED(hr));

	//------------------

	GameManager gameManager(3);

	gameManager.placeTargets(devicePtr, 0, 5);

	ID3D11ShaderResourceView *textureResourceViewPtr;
	hr = dx::CreateWICTextureFromFile(devicePtr, L"../Texture/GameOver.png", nullptr, &textureResourceViewPtr);

	assert(SUCCEEDED(hr));

	// FullScreen Quad---------------------------

	Mesh fullScreenQuad(devicePtr);
	fullScreenQuad.addVertex(-1.0f, 1.0f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	fullScreenQuad.addVertex(1.0f, -1.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f);
	fullScreenQuad.addVertex(-1.0f, -1.0f, 0.1f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f);

	fullScreenQuad.addVertex(-1.0f, 1.0f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	fullScreenQuad.addVertex(1.0f, 1.0f, 0.1f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	fullScreenQuad.addVertex(1.0f, -1.0f, 0.1f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f);

	fullScreenQuad.createVertexBuffer();

	//---------------------------

	std::vector<Mesh*> scene;

	Mesh *box = new Mesh(devicePtr);
	
	box->readModelFromeFile("../box.obj");
	box->readTextureFromeFile(L"../Texture/bump01.png");
	box->readNormalTextureFromeFile(L"../Texture/stone01.png");
	box->setTranslation(dx::XMFLOAT3(0.0f, 0.0f, 0.0f));
	box->setScale(dx::XMFLOAT3(1, 1, 1));
	box->setRotation(dx::XMFLOAT3(0, 0, 0));

	MaterialBuffer material;
	material.kd = box->getMaterial().getDiffuse();
	material.ka = box->getMaterial().getAmbient();
	//material.ks = box.getMaterial().getSpecular();
	material.ks = dx::XMFLOAT3(1,1,1);
	material.ns = 32;
	scene.push_back(box);

	//

	Mesh *character = new Mesh(devicePtr);

	character->readModelFromeFile("../character.obj");
	character->readTextureFromeFile(L"../Texture/texture map.png");
	character->setTranslation(dx::XMFLOAT3(3.0f, 0.0f, 3.0f));
	character->setScale(dx::XMFLOAT3(1, 1, 1));
	character->setRotation(dx::XMFLOAT3(0, 0, 0));
	scene.push_back(character);


	//Material------------------------------
	ID3D11Buffer *constantBufferPtrMaterial = nullptr;
	{
		D3D11_BUFFER_DESC constantBuffDescrMaterial = {};
		constantBuffDescrMaterial.ByteWidth = sizeof(MaterialBuffer);
		constantBuffDescrMaterial.Usage = D3D11_USAGE_DEFAULT; // helps the driver optimisation, based on how often the buffer values will change
		constantBuffDescrMaterial.BindFlags = D3D11_BIND_CONSTANT_BUFFER; //binding a buffer of type to the BindFlags

		D3D11_SUBRESOURCE_DATA csdDataMaterial = { 0 };
		csdDataMaterial.pSysMem = &material;
		HRESULT hr = devicePtr->CreateBuffer(&constantBuffDescrMaterial, &csdDataMaterial, &constantBufferPtrMaterial);
		assert(SUCCEEDED(hr));
	}
	
	//_______Camera_______

	Camera camera;
	camera.setPosition(dx::XMFLOAT4(0.0f, 2.0f, -2.0f, 0.0f)); // eye pos
	camera.setTarget(dx::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)); // center
	camera.setUp(dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f)); // y up

	camera.setViewMatrix(camera.getPosition(), camera.getTarget(), camera.getUp());
	camera.setAspectRatio((float)windowWidth, (float)windowHeight);
	camera.setNearZ(0.1f);
	camera.setFieldOfView(90);
	camera.setFarZ(100.0f);
	camera.setProjectionMatrix(camera.getFieldOfView(), camera.getAspectRatio(), camera.getNearZ(), camera.getFarZ());
	
	//--------------------------------------------------------

	ConstantBuffer cBufferPerObject;
	ID3D11Buffer *constantBufferPerObjectPtr = nullptr;
	createConstantBuffer(devicePtr,deviceContextPtr, cBufferPerObject, constantBufferPerObjectPtr);
		
	//--------------------------------------
	GeometryShaderBuffer geometryShaderBuffer;
	ID3D11Buffer *constantBufferPtrBackFaceCulling = nullptr;
	{
		D3D11_BUFFER_DESC constantBuffDescrGeometry = {};
		constantBuffDescrGeometry.ByteWidth = sizeof(GeometryShaderBuffer);
		constantBuffDescrGeometry.Usage = D3D11_USAGE_DEFAULT; // helps the driver optimisation, based on how often the buffer values will change
		constantBuffDescrGeometry.BindFlags = D3D11_BIND_CONSTANT_BUFFER; //binding a buffer of type to the BindFlags
		constantBuffDescrGeometry.CPUAccessFlags = 0;
		constantBuffDescrGeometry.MiscFlags = 0;
		constantBuffDescrGeometry.StructureByteStride = 0;

		HRESULT hr = devicePtr->CreateBuffer(&constantBuffDescrGeometry, nullptr, &constantBufferPtrBackFaceCulling);
		assert(SUCCEEDED(hr));
	}
	//--------------------------------------

	Terrain terrain(devicePtr);
	terrain.readTextureFromFile(L"../Texture/bump01.png");
	terrain.generateTerrainMap("../Texture/heightmapOrg1.png", 50.0f);

	terrain.calculateAverageNormal();
	terrain.createVertexAndIndexBuffers();
	terrain.setTranslation(dx::XMFLOAT3(35.0f,0.0f,0.0f));
	terrain.setScale(dx::XMFLOAT3(1.0f,1.0f,1.0f));
	terrain.setRotation(dx::XMFLOAT3(0.0f,0.0f,0.0f));
	terrain.setWorldMatrix();

	terrain.getMaterial().setAmbient(dx::XMFLOAT3(1, 1, 1));
	terrain.getMaterial().setDiffuse(dx::XMFLOAT3(0.5f, 0.5f, 0.5f));
	terrain.getMaterial().setSpecular(dx::XMFLOAT3(1.0f, 1.0f, 1.0f));
	terrain.getMaterial().setNs(32);

	MaterialBuffer terrainMaterialBuffer;
	terrainMaterialBuffer.kd = terrain.getMaterial().getDiffuse();
	terrainMaterialBuffer.ka = terrain.getMaterial().getAmbient();
	terrainMaterialBuffer.ks = terrain.getMaterial().getSpecular();
	terrainMaterialBuffer.ns = terrain.getMaterial().getNs();

	Terrain water(devicePtr);
	water.readTextureFromFile(L"../Texture/water.png");
	water.generatePlane(5, 5);
	water.createVertexAndIndexBuffers();
	water.setTranslation(dx::XMFLOAT3(1.0f, 0.0f, 0.0f));

	water.setScale(dx::XMFLOAT3(3.0f, 3.0f, 3.0f));

	water.setRotation(dx::XMFLOAT3(0.0f, 0.0f, 0.0f));
	water.setWorldMatrix();

	water.getMaterial().setAmbient(dx::XMFLOAT3(1, 1, 1));
	water.getMaterial().setDiffuse(dx::XMFLOAT3(0.5f, 0.5f, 0.5f));
	water.getMaterial().setSpecular(dx::XMFLOAT3(1.0f, 1.0f, 1.0f));
	water.getMaterial().setNs(32);

	terrainMaterialBuffer.kd = water.getMaterial().getDiffuse();
	terrainMaterialBuffer.ka = water.getMaterial().getAmbient();
	terrainMaterialBuffer.ks = water.getMaterial().getSpecular();
	terrainMaterialBuffer.ns = water.getMaterial().getNs();


	//--------------------------------------------------------
	Light lightObj;

	lightObj.setPos(dx::XMFLOAT3(10.0f, 10.0f, 0.0f));
	lightObj.setAmb(dx::XMFLOAT3(0.5f, 0.5f, 0.5f));
	lightObj.setdiff(dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	lightObj.setAtt(dx::XMFLOAT3(0.6f, 0.3f, 0.1f));
	lightObj.setGlobalAmb(1.0f);
	lightObj.getCamera().setTarget(dx::XMFLOAT4(0, 0, 0, 0));
	lightObj.getCamera().setEverythingProjection(windowWidth, windowHeight, 10.0f, 10.0f, 0.1f, 100.0f, 90);
	lightObj.setDir(lightObj.getCamera().cameraToTargetVector());

	

	LightBuffer lightBuffer;
	lightBuffer.position = lightObj.getPos();
	lightBuffer.ambientColor = lightObj.getAmb();
	lightBuffer.diffuse = lightObj.getdiff();
	lightBuffer.direction = lightObj.getDir();
	lightBuffer.attenuation = lightObj.getAtt();
	lightBuffer.globalAmbient = lightObj.getGlobalAmb();

	std::cout << "pos " << lightBuffer.position.x << "  " << lightBuffer.position.y << "  " << lightBuffer.position.z << std::endl;
	std::cout << "ambientColor " << lightBuffer.ambientColor.x << "  " << lightBuffer.ambientColor.y << "  " << lightBuffer.ambientColor.z << std::endl;
	std::cout << "diffuse " << lightBuffer.diffuse.x << "  " << lightBuffer.diffuse.y << "  " << lightBuffer.diffuse.z << "  " << lightBuffer.diffuse.w << std::endl;
	std::cout << "direction " << lightBuffer.direction.x << "  " << lightBuffer.direction.y << "  " << lightBuffer.direction.z << std::endl;
	std::cout << "attenuation " << lightBuffer.attenuation.x << "  " << lightBuffer.attenuation.y << "  " << lightBuffer.attenuation.z << std::endl;
	std::cout << "globalAmbient " << lightBuffer.globalAmbient << std::endl;

	

	//--------------------------------------------------------

	ID3D11Buffer *constantBufferPtrLight = nullptr;
	{
		D3D11_BUFFER_DESC constantBuffDescrLight = {};
		constantBuffDescrLight.ByteWidth = sizeof(LightBuffer);
		constantBuffDescrLight.Usage = D3D11_USAGE_DEFAULT; // helps the driver optimisation, based on how often the buffer values will change
		constantBuffDescrLight.BindFlags = D3D11_BIND_CONSTANT_BUFFER; //binding a buffer of type to the BindFlags

		D3D11_SUBRESOURCE_DATA csdDataLight = { 0 }; // points to our array of floats
		csdDataLight.pSysMem = &lightBuffer;
		HRESULT hr = devicePtr->CreateBuffer(&constantBuffDescrLight, &csdDataLight, &constantBufferPtrLight);
		assert(SUCCEEDED(hr));//gives us a pointer to a vertex buffer.
	}
	//--------------------------------------------------------
	
	// shadow mapping
	ShadowMapping shadowMapping(devicePtr, windowWidth, windowHeight);

	ID3D11SamplerState * samplerStateClampPtr;
	createSamplerStateClapp(devicePtr, samplerStateClampPtr, hr);
	   
	ShowWindow(hwnd, nCmdShow);
	std::cout << "before" << std::endl;

	while (!shouldClose)
	{
		static Timer timer;

		/**** handle user input and other window events ****/
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
		{
			break;
		}
		// update code here ------------------------------------------------------------------
		/*** TODO: RENDER A FRAME HERE WITH DIRECT3D ***/
		
		mouseKeyboardInput.keyboardInput(camera, lightObj, cBufferPerObject, *box, terrain);

		mouseKeyboardInput.mouseInput(camera, hwnd, gameManager.getGameTargets(), gameManager);

		speedvar = speedvar + 0.005f;

		speed = dx::XMFLOAT4(0, speedvar, 0, 0);
		//speed = dx::XMFLOAT4(0, 0, 0, 0);

		box->setRotation(dx::XMFLOAT3(0, speedvar, 0));

		gameManager.moveGameTargets();

		camera.update();

		//light update
		lightBuffer.direction = lightObj.getCamera().cameraToTargetVector();
		lightObj.getCamera().setViewMatrix();
	
		deviceContextPtr->UpdateSubresource(constantBufferPtrLight, 0, nullptr, &lightBuffer, 0, 0);
		deviceContextPtr->VSSetConstantBuffers(2, 1, &constantBufferPtrLight);
		deviceContextPtr->PSSetConstantBuffers(2, 1, &constantBufferPtrLight);

		//--------------------------------------------------------
		/***** Input Assembler (map how the vertex shader inputs should be read from vertex buffer) ******/
		deviceContextPtr->RSSetViewports(1, &viewPort);
		deviceContextPtr->IASetInputLayout(inputLayoutPtr);
		deviceContextPtr->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContextPtr->RSSetState(rasterizerStatePtr);
		deviceContextPtr->OMSetDepthStencilState(depthStencileState, 0);

		//-----------------------------------------

		// firstPass	
		/* clear the back buffer to cornflower blue for the new frame */
		float backGroundColor[4] = { 0x64 / 255.0f,0x95 / 255.0f,0xED / 255.0f,1.0f };

		deferredRender.setRenderTargets(deviceContextPtr, depthStencilViewPtr);
		deferredRender.clearRenderTargets(deviceContextPtr, backGroundColor);

		deviceContextPtr->ClearDepthStencilView(depthStencilViewPtr, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
		deviceContextPtr->ClearRenderTargetView(renderTargetViewPtr, backGroundColor);

		deviceContextPtr->VSSetShader(firstPassVertexShaderPtr, nullptr, 0);
		deviceContextPtr->PSSetShader(firstPassPixelShaderPtr, nullptr, 0);

		for (size_t i = 0; i < gameManager.getGameTargets().size(); i++)
		{
			drawDeferred(*gameManager.getGameTargets()[i], camera, lightObj, deviceContextPtr, firstPassVertexShaderPtr, firstPassPixelShaderPtr, samplerStatePtr, constantBufferPerObjectPtr, cBufferPerObject, constantBufferPtrMaterial, material);

		}
		
		drawDeferred(*scene[0], camera, lightObj, deviceContextPtr, firstPassVertexShaderPtr, firstPassPixelShaderPtr, samplerStatePtr, constantBufferPerObjectPtr, cBufferPerObject, constantBufferPtrMaterial, material);
		drawDeferred(*scene[1], camera, lightObj, deviceContextPtr, firstPassVertexShaderPtr, firstPassPixelShaderPtr, samplerStatePtr, constantBufferPerObjectPtr, cBufferPerObject, constantBufferPtrMaterial, material);

		terrain.drawDeferred(camera, lightObj, deviceContextPtr, geometryShaderPtr ,samplerStatePtr, constantBufferPerObjectPtr, cBufferPerObject, constantBufferPtrMaterial, terrainMaterialBuffer, constantBufferPtrBackFaceCulling, geometryShaderBuffer);
		water.drawDeferred(camera, lightObj, deviceContextPtr, samplerStatePtr, constantBufferPerObjectPtr, cBufferPerObject, constantBufferPtrMaterial, terrainMaterialBuffer, speed);

		ID3D11RenderTargetView* nullRenderTargetViewPtr[] = {
			nullptr,
			nullptr,
			nullptr,
			
		};
		deferredRender.setRenderTargets(deviceContextPtr, nullRenderTargetViewPtr, nullptr); //<--- THIS is needed because you cannot bind a texture and have it bound as a render target. Must release first

		// shadowpass

		shadowMapping.BindDsvAndSetNullRenderTarget(deviceContextPtr);
		drawShadow(*scene[0], camera, lightObj, deviceContextPtr, shadowPassVertexShaderPtr, constantBufferPerObjectPtr, cBufferPerObject);
		drawShadow(*scene[1], camera, lightObj, deviceContextPtr, shadowPassVertexShaderPtr, constantBufferPerObjectPtr, cBufferPerObject);

		terrain.drawShadow(camera, lightObj, deviceContextPtr, shadowPassVertexShaderPtr, constantBufferPerObjectPtr, cBufferPerObject);

		for (size_t i = 0; i < gameManager.getGameTargets().size(); i++)
		{
			drawShadow(*gameManager.getGameTargets()[i], camera, lightObj, deviceContextPtr, shadowPassVertexShaderPtr, constantBufferPerObjectPtr, cBufferPerObject);
		}

		deviceContextPtr->OMSetRenderTargets(1, &renderTargetViewPtr, depthStencilViewPtr);


		// secondPass

		deviceContextPtr->VSSetShader(secondPassVertexShaderPtr, nullptr, 0);
		deviceContextPtr->PSSetShader(secondPassPixelShaderPtr, nullptr, 0);

		deviceContextPtr->PSSetShaderResources(3, 1, &shadowMapping.getDepthMapShaderResourceView());
		deviceContextPtr->PSSetSamplers(1, 1, &samplerStateClampPtr);
		
		if (gameManager.getGameOver() == false)
		{
			deviceContextPtr->PSSetShaderResources(0, 1, &deferredRender.getShaderResourceView(0)); //Position
			deviceContextPtr->PSSetShaderResources(1, 1, &deferredRender.getShaderResourceView(1)); // Diffuse
			deviceContextPtr->PSSetShaderResources(2, 1, &deferredRender.getShaderResourceView(2)); // Normal
		}
		else if(gameManager.getGameOver() == true)
		{
			deviceContextPtr->PSSetShaderResources(0, 1, &textureResourceViewPtr); //Position
			deviceContextPtr->PSSetShaderResources(1, 1, &textureResourceViewPtr); // Diffuse
			deviceContextPtr->PSSetShaderResources(2, 1, &textureResourceViewPtr); // Normal
		}

		deviceContextPtr->IASetVertexBuffers(0, 1, &fullScreenQuad.vertexBufferPtr, &fullScreenQuad.getVertexStride(), &fullScreenQuad.getVertexOffset());
		deviceContextPtr->Draw(fullScreenQuad.getVertexCount(), 0);

		ID3D11ShaderResourceView *shaderResourceViewNullPtr = nullptr;
		deviceContextPtr->PSSetShaderResources(0, 1, &shaderResourceViewNullPtr);
		deviceContextPtr->PSSetShaderResources(1, 1, &shaderResourceViewNullPtr);
		deviceContextPtr->PSSetShaderResources(2, 1, &shaderResourceViewNullPtr);
		deviceContextPtr->PSSetShaderResources(3, 1, &shaderResourceViewNullPtr);

		//---------------------------------


		/**** swap the back and front buffers (show the frame we just drew) ****/
		swapChainPtr->Present(1, 0);

		timer.currentTime();
		timer.getDuration();
		//timer.printDuration();

	}
	ID3D11RenderTargetView * nullViews[] = { nullptr };
	deviceContextPtr->OMSetRenderTargets(1, nullViews, nullptr);

	swapChainPtr->Release();
	swapChainPtr = nullptr;

	devicePtr->Release();
	devicePtr = nullptr;

	deviceContextPtr->Release();
	deviceContextPtr = nullptr;

	renderTargetViewPtr->Release();
	renderTargetViewPtr = nullptr;

	if (vertexShaderPtr != nullptr)
	{
		vertexShaderPtr->Release();
		vertexShaderPtr = nullptr;
	}

	if (pixelShaderPtr != nullptr)
	{
		pixelShaderPtr->Release();
		pixelShaderPtr = nullptr;
	}

	if (vsBlobPtr != nullptr)
	{
		vsBlobPtr->Release();
		vsBlobPtr = nullptr;
	}

	if (psBlobPtr != nullptr)
	{
		psBlobPtr->Release();
		psBlobPtr = nullptr;
	}
	
	if (errorBlob != nullptr)
	{
		errorBlob->Release();
		errorBlob = nullptr;
	}

	inputLayoutPtr->Release();
	inputLayoutPtr = nullptr;

	depthStencilViewPtr->Release();
	depthStencilViewPtr = nullptr;

	depthStencilTexture->Release();
	depthStencilTexture = nullptr;

	depthStencileState->Release();
	depthStencileState = nullptr;

	constantBufferPerObjectPtr->Release();
	constantBufferPerObjectPtr = nullptr;
	samplerStatePtr->Release();
	samplerStatePtr = nullptr;
	samplerStateClampPtr->Release();
	samplerStateClampPtr = nullptr;

	rasterizerStatePtr->Release();
	rasterizerStatePtr = nullptr;
	constantBufferPtrLight->Release();
	constantBufferPtrLight = nullptr;
	constantBufferPtrMaterial->Release();
	constantBufferPtrMaterial = nullptr;
	constantBufferPtrBackFaceCulling->Release();
	constantBufferPtrBackFaceCulling = nullptr;

	geometryShaderPtr->Release();
	geometryShaderPtr = nullptr;
	gsBlobPtr->Release();
	gsBlobPtr = nullptr;
	
	if (gsErrorBlob != nullptr)
	{
		gsErrorBlob->Release();
		gsErrorBlob = nullptr;
	}

	textureResourceViewPtr->Release();
	textureResourceViewPtr = nullptr;
	deferredRender.release();

	firstPassVertexShaderPtr->Release();
	firstPassVertexShaderPtr = nullptr;
	firstPassPixelShaderPtr->Release();
	firstPassPixelShaderPtr = nullptr;
	vsFirstPassBlobPtr->Release();
	vsFirstPassBlobPtr = nullptr;
	psFirstPassBlobPtr->Release();
	psFirstPassBlobPtr = nullptr;
	errorFirstPassBlob->Release();
	errorFirstPassBlob = nullptr;
	secondPassVertexShaderPtr->Release();
	secondPassVertexShaderPtr = nullptr;
	secondPassPixelShaderPtr->Release();
	secondPassPixelShaderPtr = nullptr;
	vsSecondPassBlobPtr->Release();
	vsSecondPassBlobPtr = nullptr;
	psSecondPassBlobPtr->Release();
	psSecondPassBlobPtr = nullptr;
	errorSecondPassBlob->Release();
	errorSecondPassBlob = nullptr;
	shadowPassVertexShaderPtr->Release();
	shadowPassVertexShaderPtr = nullptr;
	vsShadowPassBlobPtr->Release();
	vsShadowPassBlobPtr = nullptr;

	if (errorShadowPassBlob != nullptr)
	{
		errorShadowPassBlob->Release();
		errorShadowPassBlob = nullptr;
	}

	shadowMapping.release();
	
	fullScreenQuad.release();

	box->release();
	delete box;

	character->release();
	delete character;
	
	terrain.release();
	water.release();

	gameManager.release();

	mouseKeyboardInput.release();

	return(int)msg.wParam;
}

HWND InitWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX); // size in ram
	wcex.style = CS_HREDRAW | CS_VREDRAW; // window style
	wcex.lpfnWndProc = WndProc; // WndProc
	wcex.hInstance = hInstance; // Handle to the instance
	wcex.lpszClassName = L"BasicWindow"; // class name

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(nullptr, L"Error RegisterClassEx", L"Error", MB_OK);
		{
			return nullptr;
		}
	}

	RECT rc = { 0,0,windowWidth,windowHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND handle = CreateWindow
	(
		L"BasicWindow", // class name
		L"Hello Triangle", // window title
		WS_OVERLAPPEDWINDOW, //style
		CW_USEDEFAULT, // window position x
		CW_USEDEFAULT, // window position y
		rc.right - rc.left, // window width
		rc.bottom - rc.top, // window height
		nullptr, // no parent window
		nullptr, // no menu
		hInstance, // handle to instance
		nullptr // no params passed
	);

	return handle;

}


float rotationAngle(float inAngle)
{

	float angle = (inAngle / 360.0f)*dx::XM_2PI;
	return angle;
}


void printCamera(Camera & camera)
{
	std::cout << "Camera" << std::endl;
	std::cout << "X: " << dx::XMVectorGetX(camera.getPosition())
		<< " Y: " << dx::XMVectorGetY(camera.getPosition())
		<< " Z: " << dx::XMVectorGetZ(camera.getPosition())
		<< std::endl;

	std::cout << "Target" << std::endl;
	std::cout << "X: " << dx::XMVectorGetX(camera.getTarget())
		<< " Y: " << dx::XMVectorGetY(camera.getTarget())
		<< " Z: " << dx::XMVectorGetZ(camera.getTarget())
		<< std::endl;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//HWND hwnd. The windows hangle
	//UINT message. The messages code
	//WPARAM wParam. Extra message info
	//LPARAM lParam. Extra message info

	switch (message)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		} break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}