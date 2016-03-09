#pragma once
#include <Windows.h>
#include "Model.h"

void InitD3D(HWND hWnd);	// set up and init d3d11
void RenderFrame(void);		// render single frame
void CleanD3D(void);		// clean up
//void InitGraphics(void);	// creates the shapes to render
void InitPipeline1(void); 
void InitPipeline2(void); // loads and prepares the shaders
void CreateConstantBufferForP1(Model model, bool hasModel); // Create constant buffer and matrices
void CreateConstantBufferForP2(void); // Create constant buffer and matrices

void InitTerrain();
void InitModelGraphics();
void InitQuad();