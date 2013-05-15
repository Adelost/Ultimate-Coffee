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
	float ambient;
	Vector3 dlDirection;
	Vector3 dlColor;
	float padding;
};