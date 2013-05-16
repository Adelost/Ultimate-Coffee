#pragma once

// Rename these to avoid confusion with actual buffers?
// Maybe add "Data"?

struct CBPerObject 
{
	Matrix WVP;
	Matrix world;
};


struct CBPerFrame
{
	Vector4 dlDirection;
	Vector4 dlColor;

	Vector4 plPosition[MAX_POINTLIGHTS]; // See shader file define for size
	Vector4 plColor[MAX_POINTLIGHTS];
	float plRange[MAX_POINTLIGHTS];

	int drawDebug;
	float padding[3];
};


struct CBSettings
{
	bool ambient;
	bool padding[15];
};