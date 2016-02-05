#include "Project Jackalope\Functions.h"
#include <windowsx.h>
#include <d3dcompiler.h>
#include "Project Jackalope\SimpleMath.h"

#include <time.h>

#include "Project Jackalope\Player.h"
#include "Project Jackalope\Model.h"
#include "Project Jackalope\ModelLoader.h"
#include "Project Jackalope\HeightMap.h"
#include "Project Jackalope\ModelHandler.h"
#include "Project Jackalope\Light.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

using namespace DirectX;

ModelHandler modelHandler;
Light light;

Player player;
double dt;

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

//movement booleans
bool upIsPressed = false, downIsPressed = false, leftIsPressed = false, rightIsPressed = false;
DirectX::SimpleMath::Vector3 movement = DirectX::SimpleMath::Vector3(0, 0, 0);

IDXGISwapChain *swapchain;  
ID3D11Device *dev;                     
ID3D11DeviceContext *devcon;           
ID3D11RenderTargetView *backbuffer;
ID3D11Texture2D *pBackBuffer;
ID3D11InputLayout *pLayout;            
ID3D11VertexShader *pVS;              
ID3D11PixelShader *pPS;                
ID3D11Buffer *pVBuffer;
ID3D11Buffer* gConstantBuffer;
ID3D11DepthStencilView* depthStencilView;
ID3D11Texture2D* depthStencilBuffer;
ID3D11RasterizerState* rasterState;
ID3D11DepthStencilState* depthStencilState;

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

	bool failed;
	//modelHandler.addModel(ModelLoader::LoadTextFile("untitled.obj", failed));
	//modelHandler.addModel(ModelLoader::LoadTextFile("Stormtrooper.obj", failed));

	ModelLoader::HeightMapInfo hmInfo;
	ModelLoader::HeightMapLoad("heightmap.bmp", hmInfo);
	modelHandler.addModel(triangulateHeightMapData(CreateGrid(hmInfo), hmInfo));
	//CreateModelFromHeightMap(hmInfo);
	//modelHandler.addModel(hmInfo.heightMap, hmInfo.numVertices);

	light.r = 1;
	light.g = 1;
	light.b = 1;
	light.position.x = 100;
	light.position.z = 100;
	light.position.y = 1000;

	InitD3D(hWnd);

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

			player.move(movement, dt);
			movement = DirectX::SimpleMath::Vector3(0, 0, 0);
			player.update(dt);

			POINT p;
			GetCursorPos(&p);
			
			//The cursor coordinates are based on the entire screen but
			//the SCREEN_HEIGHT and SCREEN_WIDTH are hardcoded, and
			//only represent the window size, which currently is smaller
			//than the entire screen, producing suboptimal results.
			if (p.y > SCREEN_HEIGHT - (SCREEN_HEIGHT / 10))
			{
				player.rotate(0, 1, dt);
			}
			else if (p.y < (SCREEN_HEIGHT / 10))
			{
				player.rotate(0, -1, dt);
			}
			if (p.x > SCREEN_WIDTH - (SCREEN_WIDTH / 10))
			{
				player.rotate(1, 0, dt);
			}
			else if (p.x < (SCREEN_WIDTH / 10))
			{
				player.rotate(-1, 0, dt);
			}

			CreateConstantBuffer();
			RenderFrame();

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
		case 0x57:
			upIsPressed = true;
			break;
		case VK_DOWN:
		case 0x53:
			downIsPressed = true;
			break;
		case VK_RIGHT:
		case 0x44:
			rightIsPressed = true;
			break;
		case VK_LEFT:
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
		case VK_UP:
		case 0x57:
			upIsPressed = false;
			movement.z = 0;
			break;
		case VK_DOWN:
		case 0x53:	
			downIsPressed = false;
			movement.z = 0;
			break;
		case VK_RIGHT:
		case 0x44:
			rightIsPressed = false;
			movement.x = 0;
			break;
		case VK_LEFT:
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

void CreateConstantBuffer()
{
	XMMATRIX world = XMMatrixRotationY(0);
	XMMATRIX view = XMMatrixLookAtLH(player.camera, player.lookAt, XMVECTOR{ 0, 1, 0 });
	XMMATRIX proj = XMMatrixPerspectiveFovLH(3.14 * 0.45, SCREEN_WIDTH / SCREEN_HEIGHT, 0.5, 1000.0);
	XMMATRIX worldViewProj = world * view * proj;

	struct VS_CONSTANT_BUFFER
	{
		XMMATRIX worldViewProj;
		XMMATRIX world;
		XMFLOAT3 lightColor;
		XMFLOAT3 lightPosition;
	};

	VS_CONSTANT_BUFFER VsConstData;
	VsConstData.worldViewProj = worldViewProj;
	VsConstData.world = world;
	VsConstData.lightColor = XMFLOAT3{ light.r, light.g, light.b };
	VsConstData.lightPosition = light.position;

	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CONSTANT_BUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &VsConstData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	dev->CreateBuffer(&cbDesc, &InitData, &gConstantBuffer);

	angle += + 1 * dt;
	devcon->UpdateSubresource(gConstantBuffer, 0, 0, &VsConstData, 0, 0);
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

	D3D11CreateDeviceAndSwapChain(
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

	//Creating depthstencil (though forced to name it depthbuffer) and rasterdesc
	D3D11_DEPTH_STENCIL_DESC depthBufferDesc;

	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.DepthEnable = true;
	depthBufferDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthBufferDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthBufferDesc.StencilEnable = true;
	depthBufferDesc.StencilReadMask = 0xFF;
	depthBufferDesc.StencilWriteMask = 0xFF;
	depthBufferDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthBufferDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthBufferDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthBufferDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthBufferDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthBufferDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthBufferDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthBufferDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dev->CreateDepthStencilState(&depthBufferDesc, &depthStencilState);
	devcon->OMSetDepthStencilState(depthStencilState, 1);
		
	//Describe Depth/Stencil Buffer and create 
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = SCREEN_WIDTH;
	depthStencilDesc.Height = SCREEN_HEIGHT;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	dev->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	dev->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);

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

	InitPipeline();
	InitGraphics();
}

// Render single frame
void RenderFrame(void)
{
	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	devcon->ClearRenderTargetView(backbuffer, clearColor);
	devcon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	devcon->VSSetConstantBuffers(0, 1, &gConstantBuffer);

	// select which vertex buffer to display
	UINT stride = sizeof(Model::Vertex);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
	devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
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
	gConstantBuffer->Release();
	depthStencilView->Release();
	depthStencilBuffer->Release();
	rasterState->Release();
	depthStencilState->Release();

}

// Create shapes to render
void InitGraphics()
{
	// create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;                
	bd.ByteWidth = sizeof(Model::Vertex)*modelHandler.getVertices().size();           
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;   

	dev->CreateBuffer(&bd, NULL, &pVBuffer); 

	// copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    
	memcpy(ms.pData, modelHandler.getVertices().data(), sizeof(Model::Vertex)*modelHandler.getVertices().size());      
	devcon->Unmap(pVBuffer, NULL);                                    
}

// Loads and prepares the shaders
void InitPipeline()
{
	ID3D10Blob *VS, *PS;
	D3DCompileFromFile(L"Effects.fx", 0, 0, "VShader", "vs_4_0", 0, 0, &VS, 0);
	D3DCompileFromFile(L"Effects.fx", 0, 0, "PShader", "ps_4_0", 0, 0, &PS, 0);
	

	dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
	dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

	devcon->VSSetShader(pVS, 0, 0);
	devcon->PSSetShader(pPS, 0, 0);

	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	dev->CreateInputLayout(ied, 3, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
	devcon->IASetInputLayout(pLayout);
}