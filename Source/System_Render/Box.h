#pragma once

#include "Vertex.h"

namespace Shape
{
	VertexPos BoxVertices[8] = 
	{														//Index:
		{Vector3( 0.5f,  0.5,  0.5)},	//0
		{Vector3( 0.5f, -0.5,  0.5)},	//1
		{Vector3(-0.5f, -0.5,  0.5)},	//2
		{Vector3(-0.5f,  0.5,  0.5)},	//3
		{Vector3( 0.5f,  0.5, -0.5)},	//4
		{Vector3( 0.5f, -0.5, -0.5)},	//5
		{Vector3(-0.5f, -0.5, -0.5)},	//6
		{Vector3(-0.5f,  0.5, -0.5)},	//7
	};

	unsigned int BoxIndex[36] = 
	{
		// Front
		0, 1, 2,
		2, 3, 0,

		// Back
		7, 6, 5,
		5, 4, 7, 

		// Right
		4, 5, 1,
		1, 0, 4, 

		// Left
		3, 2, 6, 
		6, 7, 3,

		// Up
		4, 0, 3,
		3, 7, 4,

		// Down
		1, 5, 6,
		6, 2, 1,
	};
}