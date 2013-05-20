#pragma once

#include "SimpleMath.h"

// Incorporate DirectX namespace, to make it more user friendly, 
// and easier to type.
using namespace DirectX::SimpleMath;
using namespace DirectX;

typedef unsigned int uint;

class Math
{
public:
	static void init();
	static int randomInt(int p_value);
	static int randomInt(int p_min, int p_max);
	static float randomFloat(float p_min, float p_max);
	static Vector3 randomDirection();
	static Vector3 randomVector();
	static int round(float value);
	static void getPickingRay(int sx, int sy, XMVECTOR &rayOrigin, XMVECTOR &rayDir);
	static const float Pi;
};

class Int2
{
public:
	int x;
	int y;

public:
	Int2()
	{
		x = 0;
		y = 0;
	}
	Int2(int p_x, int p_y)
	{
		x = p_x;
		y = p_y;
	}
};

class Float2
{
public:
	float x;
	float y;

public:
	Float2()
	{
		x = 0;
		y = 0;
	}
	Float2(float p_x, float p_y)
	{
		x = p_x;
		y = p_y;
	}
	const float yRatio()
	{
		return x/y;
	}
};

class FloatRectangle
{
public:
	Float2 position;
	Float2 size;

public:
	FloatRectangle(){}
	FloatRectangle(Float2 position, Float2 size)
	{
		this->position = position;
		this->size = size;
	}

public:
	float x()
	{
		return position.x;
	}
	float y()
	{
		return position.y;
	}
	float sizeX()
	{
		return size.x;
	}
	float sizeY()
	{
		return size.y;
	}
	Float2 center()
	{
		Float2 out(x() + sizeX()/2, y() + sizeY()/2);

		return out;
	}
};

struct MyRectangle
{
	XMFLOAT3 P1, P2, P3, P4;
};

struct MyPlane
{
	XMFLOAT3 normal;
	float offset;
};