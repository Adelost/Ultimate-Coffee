#pragma once

#include <Core/stdafx.h>

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
	Vector4 color;
	Vector3 normal;
};