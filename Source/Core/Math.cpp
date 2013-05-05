#include "stdafx.h"
#include "Math.h"
#include <time.h>

const float Math::Pi = 3.1415926535f;
const float Math::Pi2 = 3.1415926535f * 2;

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
	srand((unsigned)time(NULL));
}