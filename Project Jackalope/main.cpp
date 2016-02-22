#include "Project Jackalope\Functions.h"
#include <windowsx.h>
#include <d3dcompiler.h>
#include "Project Jackalope\SimpleMath.h"

#include <time.h>

#include "Project Jackalope\Player.h"
#include "Project Jackalope\Model.h"
#include "Project Jackalope\HeightMap.h"
#include "Project Jackalope\ModelHandler.h"
#include "Project Jackalope\Light.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

using namespace DirectX;

ModelHandler modelHandler;
HeightMap hm;
HeightMap::HeightMapInfo hmInfo;
Light light;
Player player;
double dt;

#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT 1080

//movement booleans
bool upIsPressed = false, downIsPressed = false, leftIsPressed = false, rightIsPressed = false;
DirectX::SimpleMath::Vector3 movement = DirectX::SimpleMath::Vector3(0, 0, 0);

IDXGISwapChain *swapchain = nullptr;
ID3D11Device *dev = nullptr;
ID3D11DeviceContext *devcon = nullptr;
ID3D11RenderTargetView *backbuffer = nullptr;
ID3D11Texture2D *pBackBuffer = nullptr;
ID3D11InputLayout *pLayout = nullptr;
ID3D11VertexShader *pVS = nullptr;
ID3D11PixelShader *pPS = nullptr;
ID3D11Buffer *pVBuffer = nullptr;
ID3D11Buffer *pVBuffer2 = nullptr;
ID3D11Buffer *pIBuffer = nullptr;
ID3D11Buffer** gConstantBuffers = new ID3D11Buffer[2];
gConstantBuffers[0] = nullptr;
gConstantBuffers[1] = nullptr;
ID3D11DepthStencilView* depthStencilView;
ID3D11Texture2D* depthStencilBuffer = nullptr;
ID3D11RasterizerState* rasterState = nullptr;
ID3D11DepthStencilState* depthStencilState = nullptr;

float angle = 0.0f;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.lpszClassName = L"WindowClass1";

	RegisterClassEx(&wc);

	RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };  
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
														 
	hWnd = CreateWindowEx(NULL,
		L"WindowClass1",
		L"Project basic window",
		WS_OVERLAPPEDWINDOW,
		300,
		300,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		hInstance, 
		NULL); 

	ShowWindow(hWnd, nCmdShow);

	InitD3D(hWnd);

	//Load models
	bool failed;
	modelHandler.addModel(Model::LoadTextFile("cubeTex.obj", failed, dev, devcon));
	//modelHandler.addModel(Model::LoadTextFile("Stormtrooper.obj", failed));
	hm.HeightMapLoad("heightmap.bmp", hmInfo, dev);
	hm.CreateGrid(hmInfo, hm.getV(),hm.getIndices());
	hm.calculateNormals();
	//modelHandler.addModel(hmInfo.heightMap, hmInfo.numVertices);

	InitGraphics();

	light.r = 1;
	light.g = 1;
	light.b = 1;
	light.position.x = 100;
	light.position.z = 100;
	light.position.y = 200;

	MSG msg = { 0 };
	
	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			clock_t prevTime = clock();

			if(upIsPressed)
				movement += DirectX::SimpleMath::Vector3(0, 0, 1);
			if (downIsPressed)
				movement += DirectX::SimpleMath::Vector3(0, 0, -1);
			if (rightIsPressed)
				movement += DirectX::SimpleMath::Vector3(1, 0, 0);
			if (leftIsPressed)
				movement += DirectX::SimpleMath::Vector3(-1, 0, 0);

			if (movement.x != 0 || movement.y != 0 || movement.z != 0)
			{
				player.move(movement.z*dt*25);
				player.strafe(movement.x*dt*25);
			}

			movement = DirectX::SimpleMath::Vector3(0, 0, 0);
			player.update();

			POINT p;
			GetCursorPos(&p);
			
			//The cursor coordinates are based on the entire screen but
			//the SCREEN_HEIGHT and SCREEN_WIDTH are hardcoded, and
			//only represent the window size, which currently is smaller
			//than the entire screen, producing suboptimal results.
			if (p.y > SCREEN_HEIGHT - (SCREEN_HEIGHT / 100))
			{
				player.pitch(1, dt);
			}
			else if (p.y < (SCREEN_HEIGHT / 100))
			{
				player.pitch(-1, dt);
			}
			if (p.x > SCREEN_WIDTH - (SCREEN_WIDTH / 100))
			{
				player.yaw(1, dt);
			}
			else if (p.x < (SCREEN_WIDTH / 100))
			{
				player.yaw(-1, dt);
			}

			CreateConstantBufferForP1();
			RenderFrame();
			gConstantBuffers[0]->Release();
			gConstantBuffers[1]->Release();

			clock_t diffTime = clock() - prevTime;
			dt = (float)(diffTime) / CLOCKS_PER_SEC;
			prevTime = clock();
		}	
	}

	CleanD3D();

	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{	
		PostQuitMessage(0);
		return 0;
	}
	case WM_KEYDOWN:
	{
		if (wParam == VK_ESCAPE) {
			DestroyWindow(hWnd);
		}

		switch (wParam)
		{
		case VK_UP:
			light.position.z += 10;
			break;
		case 0x57:
			upIsPressed = true;
			break;
		case VK_DOWN:
			light.position.z -= 10;
			break;
		case 0x53:
			downIsPressed = true;
			break;
		case VK_RIGHT:
			light.position.x += 10;
			break;
		case 0x44:
			rightIsPressed = true;
			break;
		case VK_LEFT:
			light.position.x -= 10;
			break;
		case 0x41:
			leftIsPressed = true;
			break;
		case VK_SPACE:
			player.jump(dt);
			break;
		}
		break;
	}
	case WM_KEYUP:
	{
		switch (wParam)
		{
		case 0x57:
			upIsPressed = false;
			movement.z = 0;
			break;
		case 0x53:	
			downIsPressed = false;
			movement.z = 0;
			break;
		case 0x44:
			rightIsPressed = false;
			movement.x = 0;
			break;
		case 0x41:
			leftIsPressed = false;
			movement.x = 0;
			break;
		}
		break;
	}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

void CreateConstantBufferForP1()
{
	XMMATRIX world = XMMatrixRotationX(0);
	player.view = XMMatrixLookToLH(player.camera, player.lookAt, XMVECTOR{ 0, 1, 0 });
	XMMATRIX proj = XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60), SCREEN_WIDTH / SCREEN_HEIGHT, 0.5, 1000.0);
	XMMATRIX worldViewProj = world * player.view * proj;
	
	struct VS_TRANSFORMS_BUFFER
	{
		XMMATRIX worldViewProj;
		XMMATRIX world;
	};

	VS_TRANSFORMS_BUFFER TransformsData;
	TransformsData.worldViewProj = worldViewProj;
	TransformsData.world = world;

	D3D11_BUFFER_DESC cbTDesc;
	cbTDesc.ByteWidth = sizeof(VS_TRANSFORMS_BUFFER);
	cbTDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbTDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbTDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbTDesc.MiscFlags = 0;
	cbTDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitTransformData;
	InitTransformData.pSysMem = &TransformsData;
	InitTransformData.SysMemPitch = 0;
	InitTransformData.SysMemSlicePitch = 0;
	dev->CreateBuffer(&cbTDesc, &InitTransformData, &gConstantBuffers[0]);

	devcon->UpdateSubresource(gConstantBuffers[0], 0, 0, &TransformsData, 0, 0);

	struct VS_MatProperties_BUFFER
	{
		DirectX::XMFLOAT3 SpecularAlbedo;
		float SpecularPower;
	};

	VS_MatProperties_BUFFER MatPropertiesData;
	MatPropertiesData.SpecularAlbedo = DirectX::XMFLOAT3(1.0, 1.0, 1.0);
	MatPropertiesData.SpecularPower = 0.5;

	D3D11_BUFFER_DESC cbMatDesc;
	cbMatDesc.ByteWidth = sizeof(VS_TRANSFORMS_BUFFER);
	cbMatDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbMatDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbMatDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbMatDesc.MiscFlags = 0;
	cbMatDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitMatData;
	InitMatData.pSysMem = &TransformsData;
	InitMatData.SysMemPitch = 0;
	InitMatData.SysMemSlicePitch = 0;
	dev->CreateBuffer(&cbTDesc, &InitMatData, &gConstantBuffers[1]);

	devcon->UpdateSubresource(gConstantBuffers[1], 0, 0, &MatPropertiesData, 0, 0);
}

void CreateConstantBufferForP2()
{
	XMMATRIX world = XMMatrixRotationX(0);
	player.view = XMMatrixLookToLH(player.camera, player.lookAt, XMVECTOR{ 0, 1, 0 });
	XMMATRIX proj = XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60), SCREEN_WIDTH / SCREEN_HEIGHT, 0.5, 1000.0);
	XMMATRIX worldViewProj = world * player.view * proj;

	struct VS_LIGHTS_BUFFER
	{
		XMMATRIX worldViewProj;
		XMMATRIX world;
	};

	VS_LIGHTS_BUFFER LightsData;
	LightsData.worldViewProj = worldViewProj;
	LightsData.world = world;

	D3D11_BUFFER_DESC cbLightsDesc;
	cbLightsDesc.ByteWidth = sizeof(D3D11_BUFFER_DESC);
	cbLightsDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbLightsDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbLightsDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbLightsDesc.MiscFlags = 0;
	cbLightsDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitLightsData;
	InitLightsData.pSysMem = &LightsData;
	InitLightsData.SysMemPitch = 0;
	InitLightsData.SysMemSlicePitch = 0;
	dev->CreateBuffer(&cbLightsDesc, &InitLightsData, &gConstantBuffers[0]);

	devcon->UpdateSubresource(gConstantBuffers[0], 0, 0, &InitLightsData, 0, 0);

	struct VS_Camera_BUFFER
	{
		DirectX::XMFLOAT3 SpecularAlbedo;
		float SpecularPower;
	};

	VS_Camera_BUFFER CameraData;
	CameraData.SpecularAlbedo = DirectX::XMFLOAT3(1.0, 1.0, 1.0);
	CameraData.SpecularPower = 0.5;

	D3D11_BUFFER_DESC cbCameraDesc;
	cbCameraDesc.ByteWidth = sizeof(D3D11_BUFFER_DESC);
	cbCameraDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbCameraDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbCameraDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbCameraDesc.MiscFlags = 0;
	cbCameraDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitCameraData;
	InitCameraData.pSysMem = &CameraData;
	InitCameraData.SysMemPitch = 0;
	InitCameraData.SysMemSlicePitch = 0;
	dev->CreateBuffer(&cbCameraDesc, &InitCameraData, &gConstantBuffers[1]);

	devcon->UpdateSubresource(gConstantBuffers[1], 0, 0, &InitCameraData, 0, 0);
}

void InitD3D(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC scd;

	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     
	scd.BufferDesc.Width = SCREEN_WIDTH;                    
	scd.BufferDesc.Height = SCREEN_HEIGHT;                  
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;     
	scd.OutputWindow = hWnd;                              
	scd.SampleDesc.Count = 4;                               
	scd.Windowed = TRUE;                                    
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;     

	HRESULT hr2 = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&swapchain,
		&dev,
		NULL,
		&devcon);
		
	//Describe Depth/Stencil Buffer and create 
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = SCREEN_WIDTH;
	depthStencilDesc.Height = SCREEN_HEIGHT;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 4;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HRESULT hr = dev->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);

	//Creating depthstencil (though forced to name it depthbuffer) and rasterdesc
	D3D11_DEPTH_STENCIL_DESC depthBufferDesc;

	// Depth test parameters
	depthBufferDesc.DepthEnable = true;
	depthBufferDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthBufferDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test parameters
	depthBufferDesc.StencilEnable = true;
	depthBufferDesc.StencilReadMask = 0xFF;
	depthBufferDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	depthBufferDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthBufferDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthBufferDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthBufferDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	depthBufferDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthBufferDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthBufferDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthBufferDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	dev->CreateDepthStencilState(&depthBufferDesc, &depthStencilState);
	devcon->OMSetDepthStencilState(depthStencilState, 1);

	//Rasterizer description
	D3D11_RASTERIZER_DESC rasterDesc;

	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	dev->CreateRasterizerState(&rasterDesc, &rasterState);
	devcon->RSSetState(rasterState);

	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);

	// depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	descDSV.Texture2D.MipSlice = 0;
	descDSV.Flags = 0;

	hr = dev->CreateDepthStencilView(depthStencilBuffer, &descDSV, &depthStencilView);

	// Bind depth stencil view
	devcon->OMSetRenderTargets(1, &backbuffer, NULL);
	devcon->OMSetRenderTargets(1, &backbuffer, depthStencilView);

	// Set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;

	devcon->RSSetViewports(1, &viewport);

	//ShowCursor(false);

	InitPipeline1();
}

// Render single frame
void RenderFrame(void)
{
	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	devcon->ClearRenderTargetView(backbuffer, clearColor);
	devcon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	devcon->VSSetConstantBuffers(0, 2, gConstantBuffers);

	InitPipeline1();

	//select heightmap buffer
	UINT stride = sizeof(Model::Vertex);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
	devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	ID3D11ShaderResourceView* shaders1[2];
	shaders1[0] = hm.pSRV;
	shaders1[1] = hm.bmpMapView;
	devcon->PSSetShaderResources(0, 2, shaders1);

	devcon->DrawIndexed(hm.getIndices().size(), 0, 0);

	//select model buffer
	stride = sizeof(Model::Vertex);
	offset = 0;
	devcon->IASetVertexBuffers(0, 1, &pVBuffer2, &stride, &offset);
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//devcon->PSSetShaderResources(0, 1, &modelHandler.getModels()[0].pSRV);

	if(!modelHandler.getVertices().empty())
		devcon->Draw(modelHandler.getVertices().size(), 0);

	InitPipeline2();
	CreateConstantBufferForP2();

	//select heightmap buffer
	stride = sizeof(Model::Vertex);
	offset = 0;
	devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
	devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shaders1[2];
	shaders1[0] = hm.pSRV;
	shaders1[1] = hm.bmpMapView;
	devcon->PSSetShaderResources(0, 2, shaders1);

	devcon->DrawIndexed(hm.getIndices().size(), 0, 0);

	//select model buffer
	stride = sizeof(Model::Vertex);
	offset = 0;
	devcon->IASetVertexBuffers(0, 1, &pVBuffer2, &stride, &offset);
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//devcon->PSSetShaderResources(0, 1, &modelHandler.getModels()[0].pSRV);

	if (!modelHandler.getVertices().empty())
		devcon->Draw(modelHandler.getVertices().size(), 0);
	swapchain->Present(0, 0);
}

// Clean up
void CleanD3D(void)
{
	swapchain->SetFullscreenState(FALSE, NULL);		// switch to windowed mode

	pLayout->Release();
	pVS->Release();
	pPS->Release();
	pVBuffer->Release();
	swapchain->Release();
	backbuffer->Release();
	dev->Release();
	devcon->Release();
	pBackBuffer->Release();
	//gConstantBuffer->Release();
	depthStencilView->Release();
	depthStencilBuffer->Release();
	//rasterState->Release();
	depthStencilState->Release();
}

// Create shapes to render
void InitGraphics()
{
	// create the index buffer
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(DWORD) * hm.NumFaces * 3;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = hm.getIndices().data();
	iinitData.SysMemPitch = 0;
	iinitData.SysMemSlicePitch = 0;
	dev->CreateBuffer(&ibd, &iinitData, &pIBuffer);
	devcon->IASetIndexBuffer(pIBuffer, DXGI_FORMAT_R32_UINT, 0);

	// create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;                
	bd.ByteWidth = sizeof(Model::Vertex)*hmInfo.numVertices;           
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       
	bd.CPUAccessFlags = 0;   
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = hm.getV().data();
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	HRESULT hr = dev->CreateBuffer(&bd, &vertexBufferData, &pVBuffer);

	D3D11_BUFFER_DESC bd2;
	ZeroMemory(&bd2, sizeof(bd2));

	bd2.Usage = D3D11_USAGE_DYNAMIC;
	bd2.ByteWidth = sizeof(Model::Vertex)*modelHandler.getVertices().size();
	bd2.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd2.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData2;
	ZeroMemory(&vertexBufferData2, sizeof(vertexBufferData2));

	vertexBufferData2.pSysMem = modelHandler.getVertices().data();
	vertexBufferData2.SysMemPitch = 0;
	vertexBufferData2.SysMemSlicePitch = 0;
	hr = dev->CreateBuffer(&bd2, &vertexBufferData2, &pVBuffer2);

	if (!modelHandler.getVertices().empty())
	{
		D3D11_MAPPED_SUBRESOURCE ms;
		HRESULT HArgh = devcon->Map(pVBuffer2, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
		memcpy(ms.pData, modelHandler.getVertices().data(), sizeof(Model::Vertex)*modelHandler.getVertices().size());
		devcon->Unmap(pVBuffer2, NULL);
	}
}

// Loads and prepares the shaders
void InitPipeline1()
{
	ID3D10Blob *VS, *PS;
	D3DCompileFromFile(L"DefP1.fx", 0, 0, "VSMain", "vs_4_0", 0, 0, &VS, 0);
	D3DCompileFromFile(L"DefP1.fx", 0, 0, "PSMain", "ps_4_0", 0, 0, &PS, 0);
	
	dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
	dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

	devcon->VSSetShader(pVS, 0, 0);
	devcon->PSSetShader(pPS, 0, 0);

	/*
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORDS0;
	float3 Normal : NORMAL;
	float4 Tangent : TANGENT;
	*/
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "Position", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Tangent", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	
	HRESULT hr = dev->CreateInputLayout(ied, 4, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
	devcon->IASetInputLayout(pLayout);
}

// Loads and prepares the shaders
void InitPipeline2()
{
	ID3D10Blob *VS, *PS;
	//D3DCompileFromFile(L"DefP2.fx", 0, 0, "VSMain", "vs_4_0", 0, 0, &VS, 0);
	D3DCompileFromFile(L"DefP2.fx", 0, 0, "PSMain", "ps_4_0", 0, 0, &PS, 0);

	//dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
	dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

	//devcon->VSSetShader(pVS, 0, 0);
	devcon->PSSetShader(pPS, 0, 0);

	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 52, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	//dev->CreateInputLayout(ied, 0, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
	//devcon->IASetInputLayout(pLayout);
}