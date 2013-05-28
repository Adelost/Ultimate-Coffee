#pragma once

#include "MeshData.h"

class MeshLoader
{
public:
	MeshLoader();
	~MeshLoader();

	bool loadModel( std::string filename, MeshData& meshData);

private:
	// Use void* so that we don't have to forward declare ai-stuff
	Vector3* convertToVector3Array(void* aiArray, unsigned int nrOfElements, bool flipNormal = false);
	unsigned int* facesToIndices(void* aiArray, unsigned int nrOfFaces);
};