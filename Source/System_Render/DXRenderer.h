#pragma once

#include <Core/IObserver.h>
#include <Core/Math.h>

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
class Manager_3DTools;

class Buffer;

struct ConstantBuffer
{
	Matrix WVP;
};

class DXRenderer
	: public IObserver
{
private:
	HWND m_windowHandle;
	D3D11_VIEWPORT*				m_viewport_screen;
	ID3D11DepthStencilView*		m_view_depthStencil;
	ID3D11Device*				m_dxDevice;
	ID3D11DeviceContext*		m_dxDeviceContext;
	ID3D11InputLayout*			m_inputLayout;
	ID3D11PixelShader*			m_pixelShader;
	ID3D11RenderTargetView*		m_view_renderTarget;
	ID3D11Texture2D*			m_tex_depthStencil;
	ID3D11VertexShader*			m_vertexShader;
	IDXGISwapChain*				m_dxSwapChain;
	Buffer*						m_vertexBuffer;
	Buffer*						m_indexBuffer;
	Buffer*						m_WVPBuffer;

	ConstantBuffer				m_CBuffer;

	UINT m_msaa_quality;
	int m_clientHeight;
	int m_clientWidth;
	static DXRenderer* s_instance;

	Manager_3DTools *m_manager_tools;

public:
	DXRenderer();
	~DXRenderer();

	bool init(HWND p_windowHandle);
	void onEvent(IEvent* p_event);
	void renderFrame();

	bool initDX();
	void resizeDX(); 

	// With better hypothetical future structure, might not need these, but currently used for tools to draw themselves:
	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDeviceContext();
	ID3D11DepthStencilView* getDepthStencilView();
};

