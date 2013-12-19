#pragma once

#include <Core/stdafx.h>
struct ID3D11InputLayout;
struct D3D11_INPUT_ELEMENT_DESC;

struct VertexPos
{
	Vector3 pos;
};

struct VertexPosCol
{
	Vector3 pos;
	Vector4 color;
};

struct VertexPosColNorm
{
	Vector3 pos;
	Color color;
	Vector3 normal;
};

namespace Vertex
{
	struct PosCol
	{
		XMFLOAT3 Pos;
		XMFLOAT4 Col;
	};

	struct PosNormTex
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
	};
}

class VertexPosNormTanTex
{
public:
	Vector3 position;
	Vector3 normal;
	Vector3 tangentU;
	Vector2 texureCordinate;

public:
	VertexPosNormTanTex(){}
	VertexPosNormTanTex(const Vector3& p, const Vector3& n, const Vector3& t, const Vector2& uv)
		: position(p), normal(n), tangentU(t), texureCordinate(uv){}
	VertexPosNormTanTex(
		float px, float py, float pz, 
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v)
		: position(px,py,pz), normal(nx,ny,nz),
		tangentU(tx, ty, tz), texureCordinate(u,v){}
};

class InputLayoutDesc
{
public:
	//// Init like const int A::a[4] = {0, 1, 2, 3}; in .cpp file.
	//static const D3D11_INPUT_ELEMENT_DESC Basic32[3];
	//static const D3D11_INPUT_ELEMENT_DESC PosCol[2];
};

class InputLayouts
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	/*static ID3D11InputLayout* Basic32;
	static ID3D11InputLayout* PosCol;*/
};