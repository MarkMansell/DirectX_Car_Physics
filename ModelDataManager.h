#pragma once
#include <d3d11.h>
//#include <d3dx11.h>
#include <directxcolors.h>
#include "DDSTextureLoader.h"
//#include "resource.h"
#include <d3d11_1.h>
#include <vector>
#include <map>
#include <string>
#include "Model.h"

#include <fstream>
#include <sstream>
#include <iostream>


class ModelDataManager
{
public:
	ModelDataManager();
	~ModelDataManager();
	void LoadTexture(ID3D11Device* device, std::string fileName);

	ID3D11ShaderResourceView* RetrieveTexture(std::string textureName);//replace this with a function to load textures from MTL files
	MeshData* RetrieveMesh(std::string meshName);
	Material* RetrieveMaterial(std::string materialName);

	//The only method you'll need to call
	void Load(char* filename, ID3D11Device* _pd3dDevice, bool invertTexCoords = true);

	//Helper methods for the above method
	//Searhes to see if a similar vertex already exists in the buffer -- if true, we re-use that index
	bool FindSimilarVertex(const SimpleVertex& vertex, std::map<SimpleVertex, unsigned short>& vertToIndexMap, unsigned short& index);

	//Re-creates a single index buffer from the 3 given in the OBJ file
	void CreateIndices(const std::vector<XMFLOAT3>& inVertices, const std::vector<XMFLOAT2>& inTexCoords, const std::vector<XMFLOAT3>& inNormals, std::vector<unsigned short>& outIndices, std::vector<XMFLOAT3>& outVertices, std::vector<XMFLOAT2>& outTexCoords, std::vector<XMFLOAT3>& outNormals);


	//int LoadOBJ(const std::string fileName);
	void LoadMaterialLibrary(ID3D11Device* device, const std::string fileName);

	Model* ConstructModel(const std::string meshName);//This is required to create a model with default layout of materials and textures

private:
	std::map<std::string, MeshData*> _meshes;
	std::map<std::string, Material*> _materials;
	std::map<std::string, ID3D11ShaderResourceView*> _textures;

	std::map<std::string, std::vector<SubObjData>> _meshSubObjs;

	//std::vector<SubObjData> subObjs;//temporary? defines an object

	//std::vector<char[128]> _mtlLibStrings;
	std::vector<std::string> _mtlLibStrings;

};
