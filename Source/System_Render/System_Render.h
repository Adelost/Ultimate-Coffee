#pragma once

#include <Core/World.h>
#include <Core/Events.h>

#include "util.h"

static bool msaa_enable = true;

namespace System
{
	/**
	Beginning of Render System implementation.
	NOTE: Not done yet.
	*/
	class Render : public Type<Render>
	{
	private:
		DataMapper<Data::Position> m_mapper_position;
		int clientWidth;
		int clientHeight;
		ID3D11Device* dxDevice;
		ID3D11DeviceContext* dxDeviceContext;
		IDXGISwapChain* dxSwapChain;
		ID3D11RenderTargetView* view_renderTarget;
		ID3D11DepthStencilView* view_depthStencil;
		ID3D11Texture2D* tex_depthStencil;
		D3D11_VIEWPORT viewport_screen;
		UINT msaa_quality;
		bool wireframe_enable;

	public:
		Render();
		~Render();
		void setupDirectX();
		void onResize(int width, int height);
		void resizeDirectX();
		void update();
		void process();
	};
}