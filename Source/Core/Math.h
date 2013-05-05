#pragma once

#include "SimpleMath.h"

// Incorporate DirectX namespace, to make it more user friendly, 
// and easier to type.
using namespace DirectX::SimpleMath;
using namespace DirectX;

class Math
{
public:
	static void init();
	static int randomInt(int p_value);
	static int randomInt(int p_min, int p_max);
	static float randomFloat(float p_min, float p_max);
	static void getPickingRay(int sx, int sy, XMVECTOR &rayOrigin, XMVECTOR &rayDir);
	static const float Pi;
	static const float Pi2;
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