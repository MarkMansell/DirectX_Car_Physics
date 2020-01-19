#pragma once

#include <windows.h>
#include "Structures.h"


class Group{
public:
	Material* mat; //FOR LATER
	ID3D11ShaderResourceView* diffuseTex;
	ID3D11ShaderResourceView* specularTex;

	bool _transparent;

	std::string _name;
	UINT _indexStart;
	UINT _numIndices;
	int _vertexStart;

//public:
	Group(std::string name, bool textured, UINT numIndices, UINT indexStart, int vertexStart, Material* m, ID3D11ShaderResourceView* dif, ID3D11ShaderResourceView * spec);
	~Group();
	void Draw(ID3D11DeviceContext* context, MeshData* mesh);
};