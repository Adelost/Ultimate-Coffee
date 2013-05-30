#include "stdafx.h"
#include "MeshLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define SAFE_ASSIGNEMENT(sourceArray, targetVariable, defaultValue, sourceVariable)	\
if(sourceArray != nullptr)															\
{																					\
	targetVariable = sourceVariable;												\
}																					\
else																				\
{																					\
	targetVariable = defaultValue;													\
}	

MeshLoader::MeshLoader()
{

}

MeshLoader::~MeshLoader()
{

}

bool MeshLoader::loadModel(std::string filename, MeshData& meshData)
{
	Assimp::Importer importer;
	// Load scene from file
	// NOTE: Some models have another winding order, potential to change that here
	// NOTE: I have not been able to successfully load normals from the mesh so we let assimp autogenerate some for us
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | /*aiProcess_FlipWindingOrder |*/ aiProcess_GenNormals | aiProcess_CalcTangentSpace);

	if(!scene || !scene->HasMeshes())
	{
		return false;
	}

	unsigned int totalNrOfVertices	= 0;
	unsigned int totalNrOfFaces		= 0;

	for(unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		totalNrOfVertices	+= scene->mMeshes[i]->mNumVertices;
		totalNrOfFaces		+= scene->mMeshes[i]->mNumFaces;
	}

	VertexPosNormTanTex*	vertices	= new VertexPosNormTanTex[totalNrOfVertices];
	unsigned int*			indices		= new unsigned int[totalNrOfFaces * 3];

	unsigned int runningVertIndex	= 0;
	unsigned int runningIndIndex	= 0;
	unsigned int indOffset			= 0;

	float largestValue = 0.0f;

	for(unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		// Vertices

		unsigned int nrOfMeshVertices	= scene->mMeshes[i]->mNumVertices;

		Vector3* positions	= nullptr;
		Vector3* normals	= nullptr;
		Vector3* tangents	= nullptr;
		Vector3* textureUV	= nullptr;

		if(scene->mMeshes[i]->HasPositions())
		{
			aiVector3D* pos = scene->mMeshes[i]->mVertices;
			positions = convertToVector3Array(pos, nrOfMeshVertices);
		}

		if(scene->mMeshes[i]->HasNormals())
		{
			aiVector3D* norm = scene->mMeshes[i]->mNormals;
			normals = convertToVector3Array(norm, nrOfMeshVertices/*, true*/); // Some normals needs to be flipped
		}

		if(scene->mMeshes[i]->HasTangentsAndBitangents())
		{
			aiVector3D* tang = scene->mMeshes[i]->mTangents;
			tangents = convertToVector3Array(tang, nrOfMeshVertices);
		}

		if(scene->mMeshes[i]->HasTextureCoords(0))
		{
			aiVector3D* uv = scene->mMeshes[i]->mTextureCoords[0];
			textureUV = convertToVector3Array(uv, nrOfMeshVertices);
		}

		for(unsigned int j = 0; j < nrOfMeshVertices; j++)
		{
			SAFE_ASSIGNEMENT(positions, vertices[runningVertIndex].position,		Vector3(0.0f, 0.0f, 0.0f),	positions[j]);
			SAFE_ASSIGNEMENT(normals,	vertices[runningVertIndex].normal,			Vector3(0.0f, 0.0f, 0.0f),	normals[j]);
			SAFE_ASSIGNEMENT(tangents,	vertices[runningVertIndex].tangentU,		Vector3(0.0f, 0.0f, 0.0f),	tangents[j]);
			SAFE_ASSIGNEMENT(textureUV, vertices[runningVertIndex].texureCordinate, Vector2(0.0f, 0.0f),		Vector2(textureUV[j].x, textureUV[j].y));

			if(abs(positions[j].x) > largestValue)
			{
				largestValue = abs(positions[j].x);
			}
			else if(abs(positions[j].y) > largestValue)
			{
				largestValue = abs(positions[j].y);
			}
			else if(abs(positions[j].z) > largestValue)
			{
				largestValue = abs(positions[j].z);
			}

			runningVertIndex++;
		}

		// Indices

		unsigned int nrOfMeshFaces = scene->mMeshes[i]->mNumFaces;
		aiFace* faces = scene->mMeshes[i]->mFaces;
		unsigned int* inds = facesToIndices(faces, nrOfMeshFaces);

		for(unsigned int j = 0; j < nrOfMeshFaces * 3; j++)
		{
			indices[runningIndIndex] = inds[j] + indOffset;
			runningIndIndex++;
		}

		indOffset += nrOfMeshVertices;

		delete positions;	
		delete normals;
		delete tangents;
		delete textureUV;
		delete inds;
	}

	meshData.vertices.assign(&vertices[0], &vertices[totalNrOfVertices]);
	meshData.indices.assign(&indices[0], &indices[totalNrOfFaces * 3]);
	meshData.largestValue = largestValue;

	delete vertices;
	delete indices;

	return true;
}

Vector3* MeshLoader::convertToVector3Array(void* aiArray, unsigned int nrOfElements, bool flipNormal)
{
	aiVector3D* aiVectorArray = static_cast<aiVector3D*>(aiArray);
	Vector3* vertices = new Vector3[nrOfElements];

	for(unsigned int i = 0; i < nrOfElements; i++)
	{
		vertices[i].x = aiVectorArray[i].x;
		vertices[i].y = aiVectorArray[i].y;
		vertices[i].z = aiVectorArray[i].z;

		if(flipNormal)
		{
			vertices[i] = -vertices[i];
		}
	}

	return vertices;
}

unsigned int* MeshLoader::facesToIndices(void* aiArray, unsigned int nrOfFaces)
{
	aiFace* aiFaceArray = static_cast<aiFace*>(aiArray);
	unsigned int* indices = new unsigned int[nrOfFaces * 3];

	for(unsigned int i = 0; i < nrOfFaces * 3; i += 3)
	{
		indices[i]		= aiFaceArray[i/3].mIndices[0];
		indices[i + 1]	= aiFaceArray[i/3].mIndices[1];
		indices[i + 2]	= aiFaceArray[i/3].mIndices[2];
	}

	return indices;
}
