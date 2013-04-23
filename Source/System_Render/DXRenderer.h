#pragma once

//Move include elsewhere
#include <Windows.h>

#include <d3d11.h>
#include <d3dcompiler.h>

#include <vector>

// DirectX error checking macro
//#if defined(DEBUG) | defined(_DEBUG)
//	#ifndef HR
//	#define HR(x)												\
//	{															\
///*		HRESULT hr = (x);										\
//		if(FAILED(hr))											\
//		{														\
//			DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true);	\
//		}	*/													\
//	}
//	#endif
//#else
	#ifndef HR
		#define HR(x) (x)
	#endif
//#endif 

#define assert(_Expression)     ((void)0)
#define ReleaseCOM(x){if(x){x->Release(); x = 0;}}
#define SafeDelete(x) { delete x; x = 0; }

#pragma comment(lib, "d3dcompiler.lib")

class DXRenderer
{
public:
	DXRenderer();
	~DXRenderer();

	bool init(HWND windowHandle);
	void renderFrame();

private:
	int clientWidth_;
	int clientHeight_;
	HWND windowHandle_;

	ID3D11Device* dxDevice_;
	ID3D11DeviceContext* dxDeviceContext_;
	IDXGISwapChain* dxSwapChain_;

	ID3D11RenderTargetView* view_renderTarget;
	ID3D11DepthStencilView* view_depthStencil;
	ID3D11Texture2D* tex_depthStencil;
	D3D11_VIEWPORT viewport_screen;

	ID3D11VertexShader* vertexShader_;
	ID3D11PixelShader* pixelShader_;

	UINT msaa_quality;

	bool initDX();
	void resizeDX(); 
};
