#include "stdafx.h"
#include "Factory_Geometry.h"

std::vector<VertexPosColNorm> Factory_Geometry::MeshData::createVertexList_posColNorm()
{
	std::vector<VertexPosColNorm> list(vertices.size());
	for(int i=0; i<(int)vertices.size(); i++)
	{
		list[i].pos = vertices[i].position;
		list[i].color =  Color(Math::randomFloat(0.0f, 1.0f), Math::randomFloat(0.0f, 1.0f), Math::randomFloat(0.0f, 1.0f));
		list[i].normal = vertices[i].normal;
	}

	return list;
}
