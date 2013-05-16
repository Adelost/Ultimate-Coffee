#pragma once


struct ID3D11RasterizerState;
struct ID3D11BlendState;
struct ID3D11DepthStencilState;

class RenderStates
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static ID3D11RasterizerState* WireframeRS;
	static ID3D11RasterizerState* NoCullRS;
	static ID3D11RasterizerState* WireframeNoCullRS;
	 
	static ID3D11BlendState* AlphaToCoverageBS;
	static ID3D11BlendState* TransparentBS;

	static ID3D11DepthStencilState* LessEqualDSS;

	static ID3D11DepthStencilState* GreaterEqualDSS;
	static ID3D11DepthStencilState* AlwaysDSS;
};