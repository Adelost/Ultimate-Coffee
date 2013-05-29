#pragma once

#include <vector>
#include <Core/Math.h>
#include "Vertex.h"

class MeshData
{
public:
	std::vector<VertexPosNormTanTex> vertices;
	std::vector<unsigned int> indices;

public:
	void computeNormals()
	{
		for(int i=0; i<(int)indices.size(); i+=3)
		{
			int i0 = indices[i];
			int i1 = indices[i+1];
			int i2 = indices[i+2];
			Vector3 v1 = vertices[i1].position - vertices[i0].position;
			Vector3 v2 = vertices[i2].position - vertices[i0].position;
			Vector3 normal = v1.Cross(v2);
			normal.Normalize();

			vertices[i0].normal = normal;
			vertices[i1].normal = normal;
			vertices[i2].normal = normal;
		}
	}
	void randomizeColor()
	{
			
	}
	std::vector<Vector3> positionList()
	{
		std::vector<Vector3> list(vertices.size());
		for(int i=0; i<(int)list.size(); i++)
		{
			list[i] = vertices[i].position;
		}

		return list;
	}

	std::vector<unsigned int> indexList()
	{
		return indices;
	}

	std::vector<VertexPosColNorm> createVertexList_posColNorm();
};