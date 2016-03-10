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

std::vector<Model::Vertex> quadVertices;

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
ID3D11Buffer* gConstantBuffers = nullptr;
ID3D11Buffer* gBuffer = nullptr;
ID3D11DepthStencilView* depthStencilView;
ID3D11Texture2D* depthStencilBuffer = nullptr;
ID3D11RasterizerState* rasterState = nullptr;
ID3D11DepthStencilState* depthStencilState = nullptr;
ID3D11RenderTargetView* t0V = nullptr;
ID3D11RenderTargetView* t1V = nullptr;
ID3D11RenderTargetView* t2V = nullptr;
ID3D11RenderTargetView* t3V = nullptr;

std::vector<ID3D11ShaderResourceView*> SRVs(4);
std::vector<ID3D11RenderTargetView*> gBuffers;

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
	//modelHandler.addModel(Model::LoadTextFile("cubeTex.obj", failed, dev));
	modelHandler.addModel(Model::LoadTextFile("Stormtrooper.obj", failed, dev));
	hm.HeightMapLoad("heightmap.bmp", hmInfo, dev);
	hm.CreateGrid(hmInfo, hm.getV(),hm.getIndices());
	hm.calculateNormals();
	//modelHandler.addModel(hmInfo.heightMap, hmInfo.numVertices);

	D3D11_TEXTURE2D_DESC tDesc;
	tDesc.Width = SCREEN_WIDTH;
	tDesc.Height = SCREEN_HEIGHT;
	tDesc.MipLevels = tDesc.ArraySize = 1;
	tDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.Usage = D3D11_USAGE_DEFAULT;
	tDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	tDesc.CPUAccessFlags = 0;
	tDesc.MiscFlags = 0;

	ID3D11Texture2D *t0 = NULL;
	ID3D11Texture2D *t1 = NULL;
	ID3D11Texture2D *t2 = NULL;
	ID3D11Texture2D *t3 = NULL;

	HRESULT stuff = dev->CreateTexture2D(&tDesc, NULL, &t0);
	stuff = dev->CreateTexture2D(&tDesc, NULL, &t1);
	stuff = dev->CreateTexture2D(&tDesc, NULL, &t2);
	stuff = dev->CreateTexture2D(&tDesc, NULL, &t3);

	D3D11_RENDER_TARGET_VIEW_DESC tVDesc;
	tVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	tVDesc.Texture2D.MipSlice = 0;

	stuff = dev->CreateRenderTargetView(t0, &tVDesc, &t0V);
	stuff = dev->CreateRenderTargetView(t1, &tVDesc, &t1V);
	stuff = dev->CreateRenderTargetView(t2, &tVDesc, &t2V);
	stuff = dev->CreateRenderTargetView(t3, &tVDesc, &t3V);
	
	if (t0 != nullptr && t1 != nullptr && t2 != nullptr && t3 != nullptr)
	{
		t0->Release();
		t1->Release();
		t2->Release();
		t3->Release();
	}
	
	gBuffers.push_back(t0V);
	gBuffers.push_back(t1V);
	gBuffers.push_back(t2V);
	gBuffers.push_back(t3V);


	D3D11_SHADER_RESOURCE_VIEW_DESC shaderDesc;
	shaderDesc.Format = tDesc.Format;
	shaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderDesc.Texture2D.MostDetailedMip = 0;
	shaderDesc.Texture2D.MipLevels = 1;

	HRESULT srv = dev->CreateShaderResourceView(t0, &shaderDesc, &SRVs[0]);
	dev->CreateShaderResourceView(t1, &shaderDesc, &SRVs[1]);
	dev->CreateShaderResourceView(t2, &shaderDesc, &SRVs[2]);
	dev->CreateShaderResourceView(t3, &shaderDesc, &SRVs[3]);

	quadVertices.push_back(Model::Vertex(-SCREEN_WIDTH/2, -SCREEN_HEIGHT, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0));
	quadVertices.push_back(Model::Vertex(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0));
	quadVertices.push_back(Model::Vertex(-SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0));
	quadVertices.push_back(Model::Vertex(-SCREEN_WIDTH, -SCREEN_HEIGHT, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0));
	quadVertices.push_back(Model::Vertex(SCREEN_WIDTH, -SCREEN_HEIGHT, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0));
	quadVertices.push_back(Model::Vertex(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0));

	InitTerrain();
	InitModelGraphics();

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

			RenderFrame();
			gConstantBuffers->Release();

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

void CreateConstantBufferForP1(Model model, bool hasModel)
{
	XMMATRIX world = XMMatrixRotationX(0);
	player.view = XMMatrixLookToLH(player.camera, player.lookAt, XMVECTOR{ 0, 1, 0 });
	XMMATRIX proj = XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60), SCREEN_WIDTH / SCREEN_HEIGHT, 0.5, 1000.0);
	XMMATRIX worldViewProj = world * player.view * proj;
	
	struct VS_CONSTANT_BUFFER
	{
		XMMATRIX WorldViewProjMatrix;
		XMMATRIX WorldMatrix;
		XMFLOAT3 SpecularAlbedo;
		float SpecularPower;
	};

	VS_CONSTANT_BUFFER ConstantData;
	ConstantData.WorldViewProjMatrix = worldViewProj;
	ConstantData.WorldMatrix = world;
	if (hasModel)
	{
		ConstantData.SpecularAlbedo = model.mMaterial.Ka;
		ConstantData.SpecularPower = model.mMaterial.Ns;
	}
	else
	{
		ConstantData.SpecularAlbedo = XMFLOAT3{ 0.5f, 0.5f, 0.5f };
		ConstantData.SpecularPower = 0.5f;
	}

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CONSTANT_BUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &ConstantData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	dev->CreateBuffer(&cbDesc, &InitData, &gConstantBuffers);

	devcon->OMSetRenderTargets(4, gBuffers.data(), NULL);

	devcon->UpdateSubresource(gConstantBuffers, 0, 0, &ConstantData, 0, 0);
}

void CreateConstantBufferForP2()
{
	XMMATRIX world = XMMatrixRotationX(0);
	player.view = XMMatrixLookToLH(player.camera, player.lookAt, XMVECTOR{ 0, 1, 0 });
	XMMATRIX proj = XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60), SCREEN_WIDTH / SCREEN_HEIGHT, 0.5, 1000.0);
	XMMATRIX worldViewProj = world * player.view * proj;

	struct VS_CONSTANT_BUFFER
	{
		XMVECTOR cameraPos;
		XMMATRIX WorldViewProjMatrix;
	};

	VS_CONSTANT_BUFFER ConstData;
	ConstData.cameraPos = player.camera;
	ConstData.WorldViewProjMatrix = worldViewProj;

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CONSTANT_BUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &ConstData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	HRESULT hrg = dev->CreateBuffer(&cbDesc, &InitData, &gConstantBuffers);

	devcon->OMSetRenderTargets(1, &backbuffer, NULL);

	devcon->UpdateSubresource(gConstantBuffers, 0, 0, &InitData, 0, 0);
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
	//devcon->OMSetRenderTargets(1, &backbuffer, NULL);
	//devcon->OMSetRenderTargets(1, &backbuffer, depthStencilView);

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

	//swapchain->SetFullscreenState(TRUE, NULL);

	InitPipeline1();
}

// Render single frame
void RenderFrame(void)
{
	InitModelGraphics();

	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	devcon->ClearRenderTargetView(backbuffer, clearColor);
	devcon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

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

	CreateConstantBufferForP1(Model(), false);
	devcon->VSSetConstantBuffers(0, 1, &gConstantBuffers);
	devcon->DrawIndexed(hm.getIndices().size(), 0, 0);

	//select model buffer
	stride = sizeof(Model::Vertex);
	offset = 0;
	devcon->IASetVertexBuffers(0, 1, &pVBuffer2, &stride, &offset);
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	int startLocation = 0;
	for (int i = 0; i < modelHandler.getModels().size(); i++)
	{
		CreateConstantBufferForP1(modelHandler.getModels()[i], true);
		devcon->VSSetConstantBuffers(0, 1, &gConstantBuffers);
		devcon->PSSetShaderResources(0, 1, &modelHandler.getModels()[i].mPSRV);
		devcon->Draw(modelHandler.getModels()[i].mVertices.size(), startLocation);
		startLocation += modelHandler.getModels()[i].mVertices.size();
	}

	InitPipeline2();
	InitQuad();
	CreateConstantBufferForP2();

	shaders1[2];
	shaders1[0] = hm.pSRV;
	shaders1[1] = hm.bmpMapView;
	//devcon->PSSetShaderResources(0, 2, shaders1);

	//select model buffer
	stride = sizeof(Model::Vertex);
	offset = 0;
	devcon->IASetVertexBuffers(0, 1, &pVBuffer2, &stride, &offset);
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	devcon->VSSetConstantBuffers(0, 1, &gConstantBuffers);
	devcon->PSSetShaderResources(0, 1, SRVs.data());
	devcon->Draw(quadVertices.size(), 0);

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
	//gConstantBuffer->ReleaseO
	depthStencilView->Release();
	depthStencilBuffer->Release();
	//rasterState->Release();
	depthStencilState->Release();
}

void InitTerrain()
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
}
void InitModelGraphics()
{
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
	HRESULT hr = dev->CreateBuffer(&bd2, &vertexBufferData2, &pVBuffer2);

	if (!modelHandler.getVertices().empty())
	{
		D3D11_MAPPED_SUBRESOURCE ms;
		HRESULT HArgh = devcon->Map(pVBuffer2, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
		memcpy(ms.pData, modelHandler.getVertices().data(), sizeof(Model::Vertex)*modelHandler.getVertices().size());
		devcon->Unmap(pVBuffer2, NULL);
	}
}
void InitQuad()
{
	D3D11_BUFFER_DESC bd2;
	ZeroMemory(&bd2, sizeof(bd2));

	bd2.Usage = D3D11_USAGE_DYNAMIC;
	bd2.ByteWidth = sizeof(Model::Vertex)*quadVertices.size();
	bd2.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd2.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData2;
	ZeroMemory(&vertexBufferData2, sizeof(vertexBufferData2));

	vertexBufferData2.pSysMem = quadVertices.data();
	vertexBufferData2.SysMemPitch = 0;
	vertexBufferData2.SysMemSlicePitch = 0;
	HRESULT hr = dev->CreateBuffer(&bd2, &vertexBufferData2, &pVBuffer2);

	if (!quadVertices.empty())
	{
		D3D11_MAPPED_SUBRESOURCE ms;
		HRESULT HArgh = devcon->Map(pVBuffer2, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
		memcpy(ms.pData, quadVertices.data(), sizeof(Model::Vertex)*quadVertices.size());
		devcon->Unmap(pVBuffer2, NULL);
	}
}

// Loads and prepares the shaders
void InitPipeline1()
{
	if (pVS != nullptr)
		pVS->Release();
	if (pPS != nullptr)
		pPS->Release();

	ID3D10Blob *VS, *PS;
	D3DCompileFromFile(L"DefP1.fx", 0, 0, "VSMain", "vs_4_0", 0, 0, &VS, 0);
	D3DCompileFromFile(L"DefP1.fx", 0, 0, "PSMain", "ps_4_0", 0, 0, &PS, 0);

	HRESULT vsstuff = dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
	vsstuff = dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

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
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	HRESULT hr = dev->CreateInputLayout(ied, 4, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
	devcon->IASetInputLayout(pLayout);
}

// Loads and prepares the shaders
void InitPipeline2()
{
	ID3D10Blob *VS, *PS, *error;
	HRESULT hr = D3DCompileFromFile(L"DefP2.fx", 0, 0, "VSMain", "vs_4_0", 0, 0, &VS, &error);
	D3DCompileFromFile(L"DefP2.fx", 0, 0, "PSMain", "ps_4_0", 0, 0, &PS, 0);

	if (pVS != nullptr)
		pVS->Release();
	if (pPS != nullptr)
		pPS->Release();

	dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
	dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

	devcon->VSSetShader(pVS, 0, 0);
	devcon->PSSetShader(pPS, 0, 0);

	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	dev->CreateInputLayout(ied, 1, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
	devcon->IASetInputLayout(pLayout);
}