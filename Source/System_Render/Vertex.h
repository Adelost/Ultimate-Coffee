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
}

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