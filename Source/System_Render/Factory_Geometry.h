#pragma once

#include <vector>
#include <Core/Math.h>
#include "Vertex.h"

#include "MeshLoader.h"

class Factory_Geometry
{
private:
	Factory_Geometry(){}

public:
	static Factory_Geometry* instance()
	{
		static Factory_Geometry instance;
		return &instance;
	}

public:
	/**
	Creates a box centered at the origin with the given dimensions.
	*/
	void createBox(float width, float height, float depth, MeshData& meshData)
	{
		MeshLoader loader;
		loader.loadModel("../../Assets/Models/teapot.obj", meshData);

		//
		// Create the vertices.
		//

		VertexPosNormTanTex v[24];

		float w2 = 0.5f*width;
		float h2 = 0.5f*height;
		float d2 = 0.5f*depth;

		// Fill in the front face vertex data.
		v[0] = VertexPosNormTanTex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[1] = VertexPosNormTanTex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[2] = VertexPosNormTanTex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		v[3] = VertexPosNormTanTex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// Fill in the back face vertex data.
		v[4] = VertexPosNormTanTex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
		v[5] = VertexPosNormTanTex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[6] = VertexPosNormTanTex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[7] = VertexPosNormTanTex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		// Fill in the top face vertex data.
		v[8]  = VertexPosNormTanTex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[9]  = VertexPosNormTanTex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[10] = VertexPosNormTanTex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		v[11] = VertexPosNormTanTex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// Fill in the bottom face vertex data.
		v[12] = VertexPosNormTanTex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
		v[13] = VertexPosNormTanTex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[14] = VertexPosNormTanTex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[15] = VertexPosNormTanTex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		// Fill in the left face vertex data.
		v[16] = VertexPosNormTanTex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
		v[17] = VertexPosNormTanTex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
		v[18] = VertexPosNormTanTex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
		v[19] = VertexPosNormTanTex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

		// Fill in the right face vertex data.
		v[20] = VertexPosNormTanTex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		v[21] = VertexPosNormTanTex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
		v[22] = VertexPosNormTanTex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
		v[23] = VertexPosNormTanTex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

		meshData.vertices.assign(&v[0], &v[24]);

		//
		// Create the indices.
		//

		unsigned int i[36];


		// Fill in the front face index data
		i[0] = 0; i[1] = 1; i[2] = 2;
		i[3] = 0; i[4] = 2; i[5] = 3;

		// Fill in the back face index data
		i[6] = 4; i[7]  = 5; i[8]  = 6;
		i[9] = 4; i[10] = 6; i[11] = 7;

		// Fill in the top face index data
		i[12] = 8; i[13] =  9; i[14] = 10;
		i[15] = 8; i[16] = 10; i[17] = 11;

		// Fill in the bottom face index data
		i[18] = 12; i[19] = 13; i[20] = 14;
		i[21] = 12; i[22] = 14; i[23] = 15;

		// Fill in the left face index data
		i[24] = 16; i[25] = 17; i[26] = 18;
		i[27] = 16; i[28] = 18; i[29] = 19;

		// Fill in the right face index data
		i[30] = 20; i[31] = 21; i[32] = 22;
		i[33] = 20; i[34] = 22; i[35] = 23;

		meshData.indices.assign(&i[0], &i[36]);
	};

	/**
	Creates a box centered at the origin with the given dimensions.
	*/
	void createPyramid(float width, float height, float depth, MeshData& meshData)
	{
		//
		// Create the vertices.
		//

		VertexPosNormTanTex v[24];

		float w2 = 0.5f*width;
		float h2 = 0.5f*height;
		float d2 = 0.5f*depth;

		// Fill in the front face vertex data.
		v[0] = VertexPosNormTanTex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[1] = VertexPosNormTanTex(0, +h2, 0, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[2] = VertexPosNormTanTex(0, +h2, 0, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		v[3] = VertexPosNormTanTex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// Fill in the back face vertex data.
		v[4] = VertexPosNormTanTex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
		v[5] = VertexPosNormTanTex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[6] = VertexPosNormTanTex(0, +h2, 0, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[7] = VertexPosNormTanTex(0, +h2, 0, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		// Fill in the top face vertex data.
		v[8]  = VertexPosNormTanTex(0, +h2, 0, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[9]  = VertexPosNormTanTex(0, +h2, 0, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[10] = VertexPosNormTanTex(0, +h2, 0, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		v[11] = VertexPosNormTanTex(0, +h2, 0, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// Fill in the bottom face vertex data.
		v[12] = VertexPosNormTanTex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
		v[13] = VertexPosNormTanTex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[14] = VertexPosNormTanTex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[15] = VertexPosNormTanTex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		// Fill in the left face vertex data.
		v[16] = VertexPosNormTanTex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
		v[17] = VertexPosNormTanTex(0, +h2, 0, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
		v[18] = VertexPosNormTanTex(0, +h2, 0, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
		v[19] = VertexPosNormTanTex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

		// Fill in the right face vertex data.
		v[20] = VertexPosNormTanTex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		v[21] = VertexPosNormTanTex(0, +h2, 0, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
		v[22] = VertexPosNormTanTex(0, +h2, 0, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
		v[23] = VertexPosNormTanTex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

		meshData.vertices.assign(&v[0], &v[24]);

		//
		// Create the indices.
		//

		unsigned int i[36];


		// Fill in the front face index data
		i[0] = 0; i[1] = 1; i[2] = 2;
		i[3] = 0; i[4] = 2; i[5] = 3;

		// Fill in the back face index data
		i[6] = 4; i[7]  = 5; i[8]  = 6;
		i[9] = 4; i[10] = 6; i[11] = 7;

		// Fill in the top face index data
		i[12] = 8; i[13] =  9; i[14] = 10;
		i[15] = 8; i[16] = 10; i[17] = 11;

		// Fill in the bottom face index data
		i[18] = 12; i[19] = 13; i[20] = 14;
		i[21] = 12; i[22] = 14; i[23] = 15;

		// Fill in the left face index data
		i[24] = 16; i[25] = 17; i[26] = 18;
		i[27] = 16; i[28] = 18; i[29] = 19;

		// Fill in the right face index data
		i[30] = 20; i[31] = 21; i[32] = 22;
		i[33] = 20; i[34] = 22; i[35] = 23;

		meshData.indices.assign(&i[0], &i[36]);

		meshData.computeNormals();
	};

	/**
	Creates a sphere centered at the origin with the given radius.  The
	slices and stacks parameters control the degree of tessellation.
	*/
	void createSphere(float radius, unsigned int sliceCount, unsigned int stackCount, MeshData& meshData)
	{
		meshData.vertices.clear();
		meshData.indices.clear();

		//
		// Compute the vertices stating at the top pole and moving down the stacks.
		//

		// Poles: note that there will be texture coordinate distortion as there is
		// not a unique point on the texture map to assign to the pole when mapping
		// a rectangular texture onto a sphere.
		VertexPosNormTanTex topVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		VertexPosNormTanTex bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

		meshData.vertices.push_back( topVertex );

		float phiStep   = Math::Pi/stackCount;
		float thetaStep = 2.0f* Math::Pi/sliceCount;

		// Compute vertices for each stack ring (do not count the poles as rings).
		for(unsigned int i = 1; i <= stackCount-1; ++i)
		{
			float phi = i*phiStep;

			// Vertices of ring.
			for(unsigned int j = 0; j <= sliceCount; ++j)
			{
				float theta = j*thetaStep;

				VertexPosNormTanTex v;

				// spherical to cartesian
				v.position.x = radius*sinf(phi)*cosf(theta);
				v.position.y = radius*cosf(phi);
				v.position.z = radius*sinf(phi)*sinf(theta);

				// Partial derivative of P with respect to theta
				v.tangentU.x = -radius*sinf(phi)*sinf(theta);
				v.tangentU.y = 0.0f;
				v.tangentU.z = +radius*sinf(phi)*cosf(theta);

				XMVECTOR T = XMLoadFloat3(&v.tangentU);
				XMStoreFloat3(&v.tangentU, XMVector3Normalize(T));

				XMVECTOR p = XMLoadFloat3(&v.position);
				XMStoreFloat3(&v.normal, XMVector3Normalize(p));

				v.texureCordinate.x = theta / XM_2PI;
				v.texureCordinate.y = phi / XM_PI;

				meshData.vertices.push_back( v );
			}
		}

		meshData.vertices.push_back( bottomVertex );

		//
		// Compute indices for top stack.  The top stack was written first to the vertex buffer
		// and connects the top pole to the first ring.
		//

		for(unsigned int i = 1; i <= sliceCount; ++i)
		{
			meshData.indices.push_back(0);
			meshData.indices.push_back(i+1);
			meshData.indices.push_back(i);
		}

		//
		// Compute indices for inner stacks (not connected to poles).
		//

		// Offset the indices to the index of the first vertex in the first ring.
		// This is just skipping the top pole vertex.
		unsigned int baseIndex = 1;
		unsigned int ringVertexCount = sliceCount+1;
		for(unsigned int i = 0; i < stackCount-2; ++i)
		{
			for(unsigned int j = 0; j < sliceCount; ++j)
			{
				meshData.indices.push_back(baseIndex + i*ringVertexCount + j);
				meshData.indices.push_back(baseIndex + i*ringVertexCount + j+1);
				meshData.indices.push_back(baseIndex + (i+1)*ringVertexCount + j);

				meshData.indices.push_back(baseIndex + (i+1)*ringVertexCount + j);
				meshData.indices.push_back(baseIndex + i*ringVertexCount + j+1);
				meshData.indices.push_back(baseIndex + (i+1)*ringVertexCount + j+1);
			}
		}

		//
		// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
		// and connects the bottom pole to the bottom ring.
		//

		// South pole vertex was added last.
		unsigned int southPoleIndex = (unsigned int)meshData.vertices.size()-1;

		// Offset the indices to the index of the first vertex in the last ring.
		baseIndex = southPoleIndex - ringVertexCount;

		for(unsigned int i = 0; i < sliceCount; ++i)
		{
			meshData.indices.push_back(southPoleIndex);
			meshData.indices.push_back(baseIndex+i);
			meshData.indices.push_back(baseIndex+i+1);
		}
	};

	void createCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& meshData)
	{
		meshData.vertices.clear();
		meshData.indices.clear();

		//
		// Build Stacks.
		// 

		float stackHeight = height / stackCount;

		// Amount to increment radius as we move up each stack level from bottom to top.
		float radiusStep = (topRadius - bottomRadius) / stackCount;

		UINT ringCount = stackCount+1;

		// Compute vertices for each stack ring starting at the bottom and moving up.
		for(UINT i = 0; i < ringCount; ++i)
		{
			float y = -0.5f*height + i*stackHeight;
			float r = bottomRadius + i*radiusStep;

			// vertices of ring
			float dTheta = 2.0f*XM_PI/sliceCount;
			for(UINT j = 0; j <= sliceCount; ++j)
			{
				VertexPosNormTanTex vertex;

				float c = cosf(j*dTheta);
				float s = sinf(j*dTheta);

				vertex.position = Vector3(r*c, y, r*s);

				vertex.texureCordinate.x = (float)j/sliceCount;
				vertex.texureCordinate.y = 1.0f - (float)i/stackCount;

				// This is unit length.
				vertex.tangentU = Vector3(-s, 0.0f, c);

				float dr = bottomRadius-topRadius;
				XMFLOAT3 bitangent(dr*c, -height, dr*s);

				XMVECTOR T = XMLoadFloat3(&vertex.tangentU);
				XMVECTOR B = XMLoadFloat3(&bitangent);
				XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));
				XMStoreFloat3(&vertex.normal, N);

				meshData.vertices.push_back(vertex);
			}
		}

		// Add one because we duplicate the first and last vertex per ring
		// since the texture coordinates are different.
		UINT ringVertexCount = sliceCount+1;

		// Compute indices for each stack.
		for(UINT i = 0; i < stackCount; ++i)
		{
			for(UINT j = 0; j < sliceCount; ++j)
			{
				meshData.indices.push_back(i*ringVertexCount + j);
				meshData.indices.push_back((i+1)*ringVertexCount + j);
				meshData.indices.push_back((i+1)*ringVertexCount + j+1);

				meshData.indices.push_back(i*ringVertexCount + j);
				meshData.indices.push_back((i+1)*ringVertexCount + j+1);
				meshData.indices.push_back(i*ringVertexCount + j+1);
			}
		}

		buildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
		buildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
	};

	void buildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& meshData)
	{
		UINT baseIndex = (UINT)meshData.vertices.size();

		float y = 0.5f*height;
		float dTheta = 2.0f*XM_PI/sliceCount;

		// Duplicate cap ring vertices because the texture coordinates and normals differ.
		for(UINT i = 0; i <= sliceCount; ++i)
		{
			float x = topRadius*cosf(i*dTheta);
			float z = topRadius*sinf(i*dTheta);

			// Scale down by the height to try and make top cap texture coord area
			// proportional to base.
			float u = x/height + 0.5f;
			float v = z/height + 0.5f;

			meshData.vertices.push_back( VertexPosNormTanTex(x, y, z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v) );
		}

		// Cap center vertex.
		meshData.vertices.push_back( VertexPosNormTanTex(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f) );

		// Index of center vertex.
		UINT centerIndex = (UINT)meshData.vertices.size()-1;

		for(UINT i = 0; i < sliceCount; ++i)
		{
			meshData.indices.push_back(centerIndex);
			meshData.indices.push_back(baseIndex + i+1);
			meshData.indices.push_back(baseIndex + i);
		}
	}

	void buildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& meshData)
	{
		// 
		// Build bottom cap.
		//

		UINT baseIndex = (UINT)meshData.vertices.size();
		float y = -0.5f*height;

		// vertices of ring
		float dTheta = 2.0f*XM_PI/sliceCount;
		for(UINT i = 0; i <= sliceCount; ++i)
		{
			float x = bottomRadius*cosf(i*dTheta);
			float z = bottomRadius*sinf(i*dTheta);

			// Scale down by the height to try and make top cap texture coord area
			// proportional to base.
			float u = x/height + 0.5f;
			float v = z/height + 0.5f;

			meshData.vertices.push_back( VertexPosNormTanTex(x, y, z, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v) );
		}

		// Cap center vertex.
		meshData.vertices.push_back( VertexPosNormTanTex(0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f) );

		// Cache the index of center vertex.
		UINT centerIndex = (UINT)meshData.vertices.size()-1;

		for(UINT i = 0; i < sliceCount; ++i)
		{
			meshData.indices.push_back(centerIndex);
			meshData.indices.push_back(baseIndex + i);
			meshData.indices.push_back(baseIndex + i+1);
		}
	}

	void createGrid(float width, float depth, unsigned int m, unsigned int n, MeshData& meshData)
	{
		unsigned int  vertexCount = m*n;
		unsigned int  faceCount   = (m-1)*(n-1)*2;

		//
		// Create the vertices.
		//

		float halfWidth = 0.5f*width;
		float halfDepth = 0.5f*depth;

		float dx = width / (n-1);
		float dz = depth / (m-1);

		float du = 1.0f / (n-1);
		float dv = 1.0f / (m-1);

		meshData.vertices.resize(vertexCount);
		for(unsigned int  i = 0; i < m; ++i)
		{
			float z = halfDepth - i*dz;
			for(unsigned int  j = 0; j < n; ++j)
			{
				float x = -halfWidth + j*dx;

				meshData.vertices[i*n+j].position = Vector3(x, 0.0f, z);
				meshData.vertices[i*n+j].normal   = Vector3(0.0f, 1.0f, 0.0f);
				meshData.vertices[i*n+j].tangentU = Vector3(1.0f, 0.0f, 0.0f);

				// Stretch texture over grid.
				meshData.vertices[i*n+j].texureCordinate.x = j*du;
				meshData.vertices[i*n+j].texureCordinate.y = i*dv;
			}
		}

		//
		// Create the indices.
		//

		meshData.indices.resize(faceCount*3); // 3 indices per face

		// Iterate over each quad and compute indices.
		unsigned int k = 0;
		for(unsigned int i = 0; i < m-1; ++i)
		{
			for(unsigned int j = 0; j < n-1; ++j)
			{
				meshData.indices[k]   = i*n+j;
				meshData.indices[k+1] = i*n+j+1;
				meshData.indices[k+2] = (i+1)*n+j;

				meshData.indices[k+3] = (i+1)*n+j;
				meshData.indices[k+4] = i*n+j+1;
				meshData.indices[k+5] = (i+1)*n+j+1;

				k += 6; // next quad
			}
		}
	};
};