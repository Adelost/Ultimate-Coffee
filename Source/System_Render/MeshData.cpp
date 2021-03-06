#include "stdafx.h"
#include "MeshData.h"

std::vector<VertexPosColNorm> MeshData::createVertexList_posColNorm()
{
	//
	int number = 5;
	std::vector<VertexPosColNorm> list(vertices.size());
	for(int i=0; i<(int)vertices.size(); i++)
	{
		list[i].pos = vertices[i].position;
		list[i].color =  Color(0.4f, 0.6f, 0.9f);
		list[i].color =  Color(1.0f, 1.0f, 1.0f);
		list[i].normal = vertices[i].normal;
	}

	return list;
}