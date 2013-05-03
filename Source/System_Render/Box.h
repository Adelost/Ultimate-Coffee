#pragma once

#include "Vertex.h"

namespace Shape
{
	Vector3 BoxPos[8] = 
	{									//Index:
		Vector3( 0.5f,  0.5f,  0.5f),	//0
		Vector3( 0.5f, -0.5f,  0.5f),	//1
		Vector3(-0.5f, -0.5f,  0.5f),	//2
		Vector3(-0.5f,  0.5f,  0.5f),	//3
		Vector3( 0.5f,  0.5f, -0.5f),	//4
		Vector3( 0.5f, -0.5f, -0.5f),	//5
		Vector3(-0.5f, -0.5f, -0.5f),	//6
		Vector3(-0.5f,  0.5f, -0.5f),	//7
	};

	Vector4 BoxColor[8] = 
	{										//Index:
		Vector4(1.0f, 1.0f, 1.0f, 1.0f),	//0
		Vector4(1.0f, 0.0f, 0.0f, 1.0f),	//1
		Vector4(0.0f, 1.0f, 0.0f, 1.0f),	//2
		Vector4(0.0f, 0.0f, 1.0f, 1.0f),	//3
		Vector4(0.0f, 1.0f, 1.0f, 1.0f),	//4
		Vector4(1.0f, 1.0f, 0.0f, 1.0f),	//5
		Vector4(1.0f, 0.0f, 1.0f, 1.0f),	//6
		Vector4(0.0f, 0.0f, 0.0f, 1.0f),	//7
	};

	Vector3 BoxNormal[8] = 
	{
		Vector3( 0.0f,  1.0f,  0.0f), // 0, Up
		Vector3( 0.0f, -1.0f,  0.0f), // 1, Down
		Vector3( 1.0f,  0.0f,  0.0f), // 2, Right
		Vector3(-1.0f,  0.0f,  0.0f), // 3, Left
		Vector3( 0.0f,  0.0f, -1.0f), // 4, Back
		Vector3( 0.0f,  0.0f,  1.0f), // 5, Front
	};

	VertexPosColNorm BoxVertices[36] =
	{
		// Front
		{BoxPos[0], BoxColor[0], BoxNormal[5]},
		{BoxPos[1], BoxColor[1], BoxNormal[5]},
		{BoxPos[2], BoxColor[2], BoxNormal[5]},

		{BoxPos[2], BoxColor[2], BoxNormal[5]},
		{BoxPos[3], BoxColor[3], BoxNormal[5]},
		{BoxPos[0], BoxColor[0], BoxNormal[5]},

		// Back
		{BoxPos[7], BoxColor[7], BoxNormal[4]},
		{BoxPos[6], BoxColor[6], BoxNormal[4]},
		{BoxPos[5], BoxColor[5], BoxNormal[4]},
										   
		{BoxPos[5], BoxColor[5], BoxNormal[4]},
		{BoxPos[4], BoxColor[4], BoxNormal[4]},
		{BoxPos[7], BoxColor[7], BoxNormal[4]},

		// Right
		{BoxPos[4], BoxColor[4], BoxNormal[2]},
		{BoxPos[5], BoxColor[5], BoxNormal[2]},
		{BoxPos[1], BoxColor[1], BoxNormal[2]},

		{BoxPos[1], BoxColor[1], BoxNormal[2]},
		{BoxPos[0], BoxColor[0], BoxNormal[2]},
		{BoxPos[4], BoxColor[4], BoxNormal[2]},

		// Left
		{BoxPos[3], BoxColor[3], BoxNormal[3]},
		{BoxPos[2], BoxColor[2], BoxNormal[3]},
		{BoxPos[6], BoxColor[6], BoxNormal[3]},

		{BoxPos[6], BoxColor[6], BoxNormal[3]},
		{BoxPos[7], BoxColor[7], BoxNormal[3]},
		{BoxPos[3], BoxColor[3], BoxNormal[3]},

		// Up
		{BoxPos[4], BoxColor[4], BoxNormal[0]},
		{BoxPos[0], BoxColor[0], BoxNormal[0]},
		{BoxPos[3], BoxColor[3], BoxNormal[0]},

		{BoxPos[3], BoxColor[3], BoxNormal[0]},
		{BoxPos[7], BoxColor[7], BoxNormal[0]},
		{BoxPos[4], BoxColor[4], BoxNormal[0]},

		// Down
		{BoxPos[1], BoxColor[1], BoxNormal[1]},
		{BoxPos[5], BoxColor[5], BoxNormal[1]},
		{BoxPos[6], BoxColor[6], BoxNormal[1]},

		{BoxPos[6], BoxColor[6], BoxNormal[1]},
		{BoxPos[2], BoxColor[2], BoxNormal[1]},
		{BoxPos[1], BoxColor[1], BoxNormal[1]},
	};

	//VertexPosColNorm BoxVerticesNormals[36] = 
	//{	
	//	// Front
	//	{Vector3( 0.5f,  0.5f,  0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector3( 0.0f,  0.0f, -1.0f)},
	//	{Vector3( 0.5f, -0.5f,  0.5f), Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector3( 0.0f,  0.0f, -1.0f)},
	//	{Vector3(-0.5f, -0.5f,  0.5f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector3( 0.0f,  0.0f, -1.0f)},
	//																				    
	//	{Vector3(-0.5f, -0.5f,  0.5f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector3( 0.0f,  0.0f, -1.0f)},
	//	{Vector3(-0.5f,  0.5f,  0.5f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector3( 0.0f,  0.0f, -1.0f)},
	//	{Vector3( 0.5f,  0.5f,  0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector3( 0.0f,  0.0f, -1.0f)},
	//																				    
	//	// Back																		    
	//	{Vector3(-0.5f,  0.5f, -0.5f), Vector4(0.0f, 0.0f, 0.0f, 1.0f), Vector3( 0.0f,  0.0f,  1.0f)},
	//	{Vector3(-0.5f, -0.5f, -0.5f), Vector4(1.0f, 0.0f, 1.0f, 1.0f), Vector3( 0.0f,  0.0f,  1.0f)},
	//	{Vector3( 0.5f, -0.5f, -0.5f), Vector4(1.0f, 1.0f, 0.0f, 1.0f), Vector3( 0.0f,  0.0f,  1.0f)},
	//																				    	   
	//	{Vector3( 0.5f, -0.5f, -0.5f), Vector4(1.0f, 1.0f, 0.0f, 1.0f), Vector3( 0.0f,  0.0f,  1.0f)},
	//	{Vector3( 0.5f,  0.5f, -0.5f), Vector4(0.0f, 1.0f, 1.0f, 1.0f), Vector3( 0.0f,  0.0f,  1.0f)},
	//	{Vector3(-0.5f,  0.5f, -0.5f), Vector4(0.0f, 0.0f, 0.0f, 1.0f), Vector3( 0.0f,  0.0f,  1.0f)},

	//	// Right
	//	{Vector3( 0.5f,  0.5f, -0.5f), Vector4(0.0f, 1.0f, 1.0f, 1.0f), Vector3( 1.0f,  0.0f,  0.0f)},
	//	{Vector3( 0.5f, -0.5f, -0.5f), Vector4(1.0f, 1.0f, 0.0f, 1.0f), Vector3( 1.0f,  0.0f,  0.0f)},
	//	{Vector3( 0.5f, -0.5f,  0.5f), Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector3( 1.0f,  0.0f,  0.0f)},
	//																				    	   
	//	{Vector3( 0.5f, -0.5f,  0.5f), Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector3( 1.0f,  0.0f,  0.0f)},
	//	{Vector3( 0.5f,  0.5f,  0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector3( 1.0f,  0.0f,  0.0f)},
	//	{Vector3( 0.5f,  0.5f, -0.5f), Vector4(0.0f, 1.0f, 1.0f, 1.0f), Vector3( 1.0f,  0.0f,  0.0f)},
	//																				    
	//	// Left																		    
	//	{Vector3(-0.5f,  0.5f,  0.5f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector3(-1.0f,  0.0f,  0.0f)},
	//	{Vector3(-0.5f, -0.5f,  0.5f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector3(-1.0f,  0.0f,  0.0f)},
	//	{Vector3(-0.5f, -0.5f, -0.5f), Vector4(1.0f, 0.0f, 1.0f, 1.0f), Vector3(-1.0f,  0.0f,  0.0f)},
	//																					 	   
	//	{Vector3(-0.5f, -0.5f, -0.5f), Vector4(1.0f, 0.0f, 1.0f, 1.0f), Vector3(-1.0f,  0.0f,  0.0f)},
	//	{Vector3(-0.5f,  0.5f, -0.5f), Vector4(0.0f, 0.0f, 0.0f, 1.0f), Vector3(-1.0f,  0.0f,  0.0f)},
	//	{Vector3(-0.5f,  0.5f,  0.5f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector3(-1.0f,  0.0f,  0.0f)},
	//																						   
	//	// Up																				   
	//	{Vector3( 0.5f,  0.5f, -0.5f), Vector4(0.0f, 1.0f, 1.0f, 1.0f), Vector3( 0.0f, -1.0f,  0.0f)},
	//	{Vector3( 0.5f,  0.5f,  0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f), Vector3( 0.0f, -1.0f,  0.0f)},
	//	{Vector3(-0.5f,  0.5f,  0.5f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector3( 0.0f, -1.0f,  0.0f)},
	//																			 	   	   
	//	{Vector3(-0.5f,  0.5f,  0.5f), Vector4(0.0f, 0.0f, 1.0f, 1.0f), Vector3( 0.0f, -1.0f,  0.0f)},
	//	{Vector3(-0.5f,  0.5f, -0.5f), Vector4(0.0f, 0.0f, 0.0f, 1.0f), Vector3( 0.0f, -1.0f,  0.0f)},
	//	{Vector3( 0.5f,  0.5f, -0.5f), Vector4(0.0f, 1.0f, 1.0f, 1.0f), Vector3( 0.0f, -1.0f,  0.0f)},
	//																			 			   
	//	// Down																	 			   
	//	{Vector3( 0.5f, -0.5f,  0.5f), Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector3( 0.0f,  1.0f,  0.0f)},
	//	{Vector3( 0.5f, -0.5f, -0.5f), Vector4(1.0f, 1.0f, 0.0f, 1.0f), Vector3( 0.0f,  1.0f,  0.0f)},
	//	{Vector3(-0.5f, -0.5f, -0.5f), Vector4(1.0f, 0.0f, 1.0f, 1.0f), Vector3( 0.0f,  1.0f,  0.0f)},
	//																			 		 	   
	//	{Vector3(-0.5f, -0.5f, -0.5f), Vector4(1.0f, 0.0f, 1.0f, 1.0f), Vector3( 0.0f,  1.0f,  0.0f)},
	//	{Vector3(-0.5f, -0.5f,  0.5f), Vector4(0.0f, 1.0f, 0.0f, 1.0f), Vector3( 0.0f,  1.0f,  0.0f)},
	//	{Vector3( 0.5f, -0.5f,  0.5f), Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector3( 0.0f,  1.0f,  0.0f)},
	//};

	/////////////////////////////////

	//VertexPosCol BoxVertices[8] = 
	//{																		//Index:
	//	{Vector3( 0.5f,  0.5f,  0.5f), Vector4(1.0f, 1.0f, 1.0f, 1.0f)},	//0
	//	{Vector3( 0.5f, -0.5f,  0.5f), Vector4(1.0f, 0.0f, 0.0f, 1.0f)},	//1
	//	{Vector3(-0.5f, -0.5f,  0.5f), Vector4(0.0f, 1.0f, 0.0f, 1.0f)},	//2
	//	{Vector3(-0.5f,  0.5f,  0.5f), Vector4(0.0f, 0.0f, 1.0f, 1.0f)},	//3
	//	{Vector3( 0.5f,  0.5f, -0.5f), Vector4(0.0f, 1.0f, 1.0f, 1.0f)},	//4
	//	{Vector3( 0.5f, -0.5f, -0.5f), Vector4(1.0f, 1.0f, 0.0f, 1.0f)},	//5
	//	{Vector3(-0.5f, -0.5f, -0.5f), Vector4(1.0f, 0.0f, 1.0f, 1.0f)},	//6
	//	{Vector3(-0.5f,  0.5f, -0.5f), Vector4(0.0f, 0.0f, 0.0f, 1.0f)},	//7
	//};

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