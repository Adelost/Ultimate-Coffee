#include "stdafx.h"
#include "RenderStates.h"

ID3D11RasterizerState* RenderStates::DepthBiasedRS = 0;

ID3D11RasterizerState* RenderStates::WireframeRS = 0;
ID3D11RasterizerState* RenderStates::WireframeNoCullRS = 0;
ID3D11RasterizerState* RenderStates::NoCullRS    = 0;
	 
ID3D11BlendState*      RenderStates::AlphaToCoverageBS = 0;
ID3D11BlendState*      RenderStates::TransparentBS     = 0;

ID3D11DepthStencilState* RenderStates::LessEqualDSS = 0;

ID3D11DepthStencilState* RenderStates::GreaterEqualDSS = 0;
ID3D11DepthStencilState* RenderStates::AlwaysDSS = 0;

void RenderStates::InitAll(ID3D11Device* device)
{
	//
	// DepthBiasedRS
	//
	D3D11_RASTERIZER_DESC depthBiasdeDesc;
	ZeroMemory(&depthBiasdeDesc, sizeof(D3D11_RASTERIZER_DESC));
	depthBiasdeDesc.CullMode = D3D11_CULL_BACK;
	depthBiasdeDesc.DepthBias = 2;
	depthBiasdeDesc.FrontCounterClockwise = false;
	depthBiasdeDesc.DepthClipEnable = false;

	HR(device->CreateRasterizerState(&depthBiasdeDesc, &DepthBiasedRS));

	//
	// WireframeRS
	//
	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;

	HR(device->CreateRasterizerState(&wireframeDesc, &WireframeRS));

	//
	// WireframeRS
	//
	D3D11_RASTERIZER_DESC wireframeNoCullDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_NONE;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;

	HR(device->CreateRasterizerState(&wireframeDesc, &WireframeNoCullRS));

	//
	// NoCullRS
	//
	D3D11_RASTERIZER_DESC noCullDesc;
	ZeroMemory(&noCullDesc, sizeof(D3D11_RASTERIZER_DESC));
	noCullDesc.FillMode = D3D11_FILL_SOLID;
	noCullDesc.CullMode = D3D11_CULL_NONE;
	noCullDesc.FrontCounterClockwise = false;
	noCullDesc.DepthClipEnable = true;

	HR(device->CreateRasterizerState(&noCullDesc, &NoCullRS));

	//
	// AlphaToCoverageBS
	//

	D3D11_BLEND_DESC alphaToCoverageDesc = {0};
	alphaToCoverageDesc.AlphaToCoverageEnable = true;
	alphaToCoverageDesc.IndependentBlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].BlendEnable = false;
	alphaToCoverageDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HR(device->CreateBlendState(&alphaToCoverageDesc, &AlphaToCoverageBS));

	//
	// TransparentBS
	//

	D3D11_BLEND_DESC transparentDesc = {0};
	transparentDesc.AlphaToCoverageEnable = false;
	transparentDesc.IndependentBlendEnable = false;

	transparentDesc.RenderTarget[0].BlendEnable = true;
	transparentDesc.RenderTarget[0].SrcBlend       = D3D11_BLEND_SRC_ALPHA;
	transparentDesc.RenderTarget[0].DestBlend      = D3D11_BLEND_INV_SRC_ALPHA;
	transparentDesc.RenderTarget[0].BlendOp        = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
	transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transparentDesc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HR(device->CreateBlendState(&transparentDesc, &TransparentBS));

	//
	// LessEqualDSS
	//

	D3D11_DEPTH_STENCIL_DESC lessEqualDesc;
	lessEqualDesc.DepthEnable      = true;
	lessEqualDesc.DepthWriteMask   = D3D11_DEPTH_WRITE_MASK_ALL;
    lessEqualDesc.DepthFunc        = D3D11_COMPARISON_LESS_EQUAL; 
    lessEqualDesc.StencilEnable    = false;
	//lessEqualDesc.BackFace.StencilFunc    = D3D11_COMPARISON_EQUAL;
	//lessEqualDesc.FrontFace.StencilFunc    = D3D11_COMPARISON_EQUAL;

	HR(device->CreateDepthStencilState(&lessEqualDesc, &LessEqualDSS));

	//
	// GreaterEqualDSS
	//

	D3D11_DEPTH_STENCIL_DESC greaterEqualDesc;
	greaterEqualDesc.DepthEnable      = true;
	greaterEqualDesc.DepthWriteMask   = D3D11_DEPTH_WRITE_MASK_ALL;
	greaterEqualDesc.DepthFunc        = D3D11_COMPARISON_GREATER_EQUAL; 
	greaterEqualDesc.StencilEnable    = false;

	HR(device->CreateDepthStencilState(&greaterEqualDesc, &GreaterEqualDSS));

	//
	// AlwaysDSS
	//

	D3D11_DEPTH_STENCIL_DESC alwaysDesc;
	greaterEqualDesc.DepthEnable      = true;
	greaterEqualDesc.DepthWriteMask   = D3D11_DEPTH_WRITE_MASK_ALL;
	greaterEqualDesc.DepthFunc        = D3D11_COMPARISON_ALWAYS; 
	greaterEqualDesc.StencilEnable    = false;

	HR(device->CreateDepthStencilState(&alwaysDesc, &AlwaysDSS));
}

void RenderStates::DestroyAll()
{
	ReleaseCOM(WireframeRS);
	ReleaseCOM(WireframeNoCullRS);
	ReleaseCOM(NoCullRS);
	ReleaseCOM(AlphaToCoverageBS);
	ReleaseCOM(TransparentBS);
	ReleaseCOM(LessEqualDSS)
	ReleaseCOM(GreaterEqualDSS);
	ReleaseCOM(AlwaysDSS);
}
