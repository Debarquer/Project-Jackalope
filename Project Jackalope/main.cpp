#include "Project Jackalope\Functions.h"
#include <windowsx.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

//whaat is this i dont know anything lol i am a bad boy
IDXGISwapChain *swapchain;  
ID3D11Device *dev;                     
ID3D11DeviceContext *devcon;           
ID3D11RenderTargetView *backbuffer;
ID3D11Texture2D *pBackBuffer;
ID3D11InputLayout *pLayout;            
ID3D11VertexShader *pVS;              
ID3D11PixelShader *pPS;                
ID3D11Buffer *pVBuffer;
ID3D11DepthStencilView* depthStencilView;
ID3D11Texture2D* depthStencilBuffer;

struct TRIANGLE { float x, y, z; float r, g, b, w; };

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
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

	MSG msg = { 0 };

	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
		}
	
		RenderFrame();
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
	} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
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

	D3D11CreateDeviceAndSwapChain(NULL,
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
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	dev->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
	dev->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);

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

	InitPipeline();
	InitGraphics();
}

// Render single frame
void RenderFrame(void)
{
	float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	devcon->ClearRenderTargetView(backbuffer, clearColor);
	devcon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// select which vertex buffer to display
	UINT stride = sizeof(TRIANGLE);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
	devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	devcon->Draw(3, 0);
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
	depthStencilView->Release();
	depthStencilBuffer->Release();
}

// Create shapes to render
void InitGraphics()
{
	TRIANGLE OurVertices[] =
	{
		{ 0.0f, 0.5f, 0.0f,		1.0f, 0.0f, 0.0f, 1.0f },
		{ 0.45f, -0.5, 0.0f,	0.0f, 1.0f, 0.0f, 1.0f },
		{ -0.45f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f, 1.0f }
	};

	// create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;                
	bd.ByteWidth = sizeof(TRIANGLE) * 3;             
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;   

	dev->CreateBuffer(&bd, NULL, &pVBuffer); 

	// copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);    
	memcpy(ms.pData, OurVertices, sizeof(OurVertices));             
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
	};

	dev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
	devcon->IASetInputLayout(pLayout);
}