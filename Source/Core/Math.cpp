#include "stdafx.h"
#include "Math.h"
#include <time.h>
#include <math.h>  

const float Math::Pi = 3.1415926535f;
const float Math::Pi2 = Math::Pi*2;

int Math::randomInt( int p_value )
{
	return rand() % p_value;
}

int Math::randomInt( int p_min, int p_max )
{
	return rand() % (p_max-p_min + 1) + p_min;
}

float Math::randomFloat( float p_min, float p_max )
{
	float random = ((float)rand())/(float)RAND_MAX;
	float diff = p_max - p_min;
	float r = random * diff;
	return p_min + r;
}

void Math::init()
{
	srand((unsigned)time(nullptr));
}

DirectX::SimpleMath::Vector3 Math::randomDirection()
{
	Vector3 v(Math::randomFloat(-1.0f, 1.0f), Math::randomFloat(-1.0f, 1.0f), Math::randomFloat(-1.0f, 1.0f));
	v.Normalize();

	return v;
}

DirectX::SimpleMath::Vector3 Math::randomVector(float min, float max)
{
	Vector3 v(Math::randomFloat(min, max), Math::randomFloat(min, max), Math::randomFloat(min, max));

	return v;
}

int Math::round( float value )
{
	return int(value+0.5f);
}

DirectX::SimpleMath::Color Math::randomColor()
{
	Color v(Math::randomFloat(0.0f, 1.0f), Math::randomFloat(0.0f, 1.0f), Math::randomFloat(0.0f, 1.0f), 1.0f);

	return v;
}

float Math::powerOf( float base, float exponent )
{
	return pow(base, exponent);
}
