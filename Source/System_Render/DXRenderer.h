#pragma once

#include <Core/IObserver.h>
#include <Core/Math.h>
#include "CBuffers.h"
#include "Factory_Geometry.h"
#include "MeshData.h"

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
class Sky;
class OculusManager;

class DXRenderer
	: public IObserver
{
private:
	HWND							m_windowHandle;
	D3D11_VIEWPORT*					m_viewport_screen;
	std::vector<D3D11_VIEWPORT*>	m_viewport_stereo;

	ID3D11DepthStencilView*			m_dsv_depthStencil;
	ID3D11Device*					m_device;
	ID3D11DeviceContext*			m_context;
	class Manager_InputLayout
	{
	public:
		void clear();
		ID3D11InputLayout*			posColNorm;
		ID3D11InputLayout*			posNormTex;
	};
	Manager_InputLayout m_layout;
	OculusManager* m_oculus;

	ID3D11PixelShader*			m_pixelShader;
	ID3D11RenderTargetView*		m_rtv_renderTarget;
	ID3D11Texture2D*			m_tex_depthStencil;
	ID3D11VertexShader*			m_vertexShader;
	IDXGISwapChain*				m_swapChain;
	Buffer*						m_objectConstantBuffer;
	Buffer*						m_frameConstantBuffer;
	Sky*						m_sky;
	Sky*						m_sky2;

	CBPerObject					m_CBPerObject;
	CBPerFrame					m_CBPerFrame;
	CBSettings					m_CBSettings;
	bool						m_msaa_enable;
	unsigned int				m_msaa_quality;
	int							m_clientHeight;
	int							m_clientWidth;
	static DXRenderer*			s_instance;

	Manager_3DTools*			m_manager_tools;
	ID3D11ShaderResourceView*	m_stereo_srv;
	ID3D11DepthStencilView*		m_stereo_dsv;

	ID3D11SamplerState* m_ssDefault;


	class ScreenQuad
	{
	public:
		ScreenQuad();
		void clear();
		ID3D11ShaderResourceView* rv_texture;
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;
		
		ID3D11VertexShader* shader_vertex;
		ID3D11PixelShader* shader_pixel;
		Buffer* cb_oculus;

		ID3D11RenderTargetView*		rtv;
		ID3D11DepthStencilView*		dsv;
		ID3D11ShaderResourceView*	srv;

	};
	ScreenQuad m_screenQuad;
	CBOculus m_CBOculus;
	

	void updatePointLights();

public:
	DXRenderer();
	~DXRenderer();

	bool init(HWND p_windowHandle);
	void onEvent(Event* p_event);
	void renderFrame();

	void drawScreenQuad( ID3D11ShaderResourceView* resource );

	void buildScreenQuad();
	bool initDX();

	void createMeshBuffer( int meshId, MeshData &mesh );

	void resizeDX(); 

	// With better hypothetical future structure, might not need these, but currently used for tools to draw themselves:
	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDeviceContext();/*	ID3D11DepthStencilView* getDepthStencilView();*/

};

