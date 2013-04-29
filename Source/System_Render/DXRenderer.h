#pragma once

#include <Core/IObserver.h>
#include "Math.h"

typedef struct HWND__* HWND;
typedef unsigned int UINT;
struct D3D11_VIEWPORT;
struct ID3D11DepthStencilView;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11InputLayout;
struct ID3D11PixelShader;
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11VertexShader;
struct IDXGISwapChain;
struct ID3D11Buffer;




class DXRenderer
	: public IObserver
{
private:
	HWND m_windowHandle;
	D3D11_VIEWPORT* m_viewport_screen;
	ID3D11DepthStencilView* m_view_depthStencil;
	ID3D11Device* m_dxDevice;
	ID3D11DeviceContext* m_dxDeviceContext;
	ID3D11InputLayout* m_inputLayout;
	ID3D11PixelShader* m_pixelShader;
	ID3D11RenderTargetView* m_view_renderTarget;
	ID3D11Texture2D* m_tex_depthStencil;
	ID3D11VertexShader* m_vertexShader;
	IDXGISwapChain* m_dxSwapChain;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

	UINT m_msaa_quality;
	int m_clientHeight;
	int m_clientWidth;
	static DXRenderer* s_instance;

public:
	DXRenderer();
	~DXRenderer();

	bool init(HWND p_windowHandle);
	void onEvent(IEvent* p_event);
	void renderFrame();

	bool initDX();
	void resizeDX(); 
};

