#pragma once

// Rename these to avoid confusion with actual buffers?
// Maybe add "Data"?

struct CBPerObject 
{
	Matrix WVP;
	Matrix world;
	Color color;
};


struct CBPerFrame
{
	Vector4 dlDirectionAndAmbient;
	Vector4 dlColor;

	Vector4 plPosition[MAX_POINTLIGHTS]; // See shader file define for size
	Vector4 plColorAndRange[MAX_POINTLIGHTS];

	int drawDebug;
	float padding[3];
};


struct CBSettings
{
	bool ambient;
	bool padding[15];
};

struct CBSkybox
{
	Matrix WVP;
	Color color;
};