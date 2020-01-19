#pragma once
//#include <xnamath.h>

#include <d3d11.h>
#include <d3d11_1.h>
//#include <d3dx11.h>
#include <directxmath.h>
#include <string>
using namespace DirectX;

struct SimpleVertex{
    XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexC;

	bool operator<(const SimpleVertex other) const{ 
		return memcmp((void*)this, (void*)&other, sizeof(SimpleVertex)) > 0; 
	}; 

};

struct MeshData{
	int VertexCount, IndexCount;
	UINT VBStride, VBOffset;
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
};

struct Material{
	std::string matName;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	XMFLOAT4 specularExponent;
	float transparency;

	//std::string ambientTexName;
	std::string diffuseTexName;
	std::string specularTexName;
	Material(){
		diffuseTexName = "NOSTRING";
		specularTexName = "NOSTRING";
	};
};

struct SubObjData{
	std::string id;
	std::string useMtl;
	INT _vertexStartLocation;//probably dont need this since indices assume a single large vertex list
	UINT _vertexEndLocation;//same goes for this
	UINT _indexStartLocation;
	UINT _indexEndLocation;
	//std::vector<unsigned short> vertIndices;
	//std::vector<unsigned short> normalIndices;
	//std::vector<unsigned short> textureIndices;
	SubObjData(){
		id = "NOSTRING";
		useMtl = "NOSTRING";
	};
};