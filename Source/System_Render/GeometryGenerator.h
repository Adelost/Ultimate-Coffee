//***************************************************************************************
// GeometryGenerator.h by Frank Luna (C) 2011 All Rights Reserved.
//   
// Defines a static class for procedurally generating the geometry of 
// common mathematical objects.
//
// All triangles are generated "outward" facing.  If you want "inward" 
// facing triangles (for example, if you want to place the camera inside
// a sphere to simulate a sky), you will need to:
//   1. Change the Direct3D cull mode or manually reverse the winding order.
//   2. Invert the normal.
//   3. Update the texture coordinates and tangent vectors.
//***************************************************************************************

#ifndef GEOMETRYGENERATOR_H
#define GEOMETRYGENERATOR_H

//#include "d3dUtil.h"

#include <Core/Math.h>

class GeometryGenerator
{
public:
	struct Vertex
	{
		Vertex(){}
		Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT3& t, const XMFLOAT2& uv)
			: Position(p), Normal(n), TangentU(t), TexC(uv){}
		Vertex(
			float px, float py, float pz, 
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v)
			: Position(px,py,pz), Normal(nx,ny,nz),
			  TangentU(tx, ty, tz), TexC(u,v){}

		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT3 TangentU;
		XMFLOAT2 TexC;
	};

	struct Vertex2
	{
		Vertex2(){}
		Vertex2(const XMFLOAT3& p, const XMFLOAT4& c)
			: Position(p), Color(c) {}
		Vertex2(
			float px, float py, float pz, 
			float cr, float cg, float cb, float ca)
			: Position(px,py,pz), Color(cr,cg,cb,ca)
			{}

		XMFLOAT3 Position;
		XMFLOAT4 Color;
	};

	struct MeshData
	{
		std::vector<Vertex> Vertices;
		std::vector<UINT> Indices;
	};

	struct MeshData2
	{
		std::vector<Vertex2> Vertices;
		std::vector<UINT> Indices;
	};

	//<summary>
	// Creates a box centered at the origin with the given dimensions.
	//</summary>
	void CreateBox(float width, float height, float depth, MeshData2& meshData, char colorMode, XMMATRIX &localSpaceTransform);

	///<summary>
	/// Creates a sphere centered at the origin with the given radius.  The
	/// slices and stacks parameters control the degree of tessellation.
	///</summary>
	//void CreateSphere(float radius, UINT sliceCount, UINT stackCount, MeshData& meshData);

	///<summary>
	/// Creates a geosphere centered at the origin with the given radius.  The
	/// depth controls the level of tessellation.
	///</summary>
	//void CreateGeosphere(float radius, UINT numSubdivisions, MeshData& meshData);

	//<summary>
	//Creates a cylinder parallel to the y-axis, and centered about the origin.  
	//The bottom and top radius can vary to form various cone shapes rather than true
	//cylinders.  The slices and stacks parameters control the degree of tessellation.
	//</summary>
	void CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, XMVECTOR &color, MeshData2& meshData, XMMATRIX &localSpaceTransform);

	///<summary>
	/// Creates an mxn grid in the xz-plane with m rows and n columns, centered
	/// at the origin with the specified width and depth.
	///</summary>
	//void CreateGrid(float width, float depth, UINT m, UINT n, MeshData& meshData);

	///<summary>
	/// Creates a quad covering the screen in NDC coordinates.  This is useful for
	/// postprocessing effects.
	///</summary>
	//void CreateFullscreenQuad(MeshData& meshData);

	// 

	void createLine(XMVECTOR &endPointA, XMVECTOR &endPointB, unsigned int pointsInBetweenEndPoints, XMVECTOR &colorAtEndPointA, XMVECTOR &colorAtEndPointB, XMMATRIX &localSpaceTransform, MeshData2& meshData);

	void createLineListCircle(XMVECTOR &center, float &radius, int &degreeWiseResolution, XMVECTOR &color, MeshData2& meshData, char axis);

private:
	//void Subdivide(MeshData& meshData);
	void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, XMVECTOR &color, MeshData2& meshData, XMMATRIX &localSpaceTransform);
	void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, XMVECTOR &color, MeshData2& meshData, XMMATRIX &localSpaceTransform);
};

#endif // GEOMETRYGENERATOR_H