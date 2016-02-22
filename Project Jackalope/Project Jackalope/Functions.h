#pragma once
#include <Windows.h>

void InitD3D(HWND hWnd);	// set up and init d3d11
void RenderFrame(void);		// render single frame
void CleanD3D(void);		// clean up
void InitGraphics(void);	// creates the shapes to render
void InitPipeline(void);    // loads and prepares the shaders
void CreateConstantBuffer(void); // Create constant buffer and matrices
void CreateSampler(void);