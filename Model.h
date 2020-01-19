#pragma once
#include "Group.h"
#include <vector>

//A model should be loaded based on a modelDefinition object. This object defines the shape of the scene graph, and the meshes, materials and textures used by the groups in the scene graph.
//ModelDefinitions are generated as the loader reads in .OBJ files. A model could be initialised using ModelDefinition 'CAR', but its components could be edited later.
//Note to self: Name ModelDefinitions the same string that the .OBJ file is named.

//A model is defined as a collection of groups in a scene graph. Each group has a mesh, a material and a texture.
//Groups are unaware of their position on the scene graph.

//Model(mesh); //Uses default materials and textures
//myModel->Group->Material = MaterialMap("bluemetal"); //Can override materials

class Model{
public:
	Model(MeshData* m);
	~Model();
	void Update();
	void Draw(ID3D11DeviceContext* context);

	XMVECTOR getTranslation(){
		XMVECTOR s;
		XMVECTOR r;
		XMVECTOR t;
		XMMatrixDecompose(&s, &r, &t, XMLoadFloat4x4(&translate));
		return t;
	};

	XMFLOAT3 GetTranslateVector(){ return _translateVector; }
	void SetTranslateVector(XMFLOAT3 pos){ OldVector = _translateVector; _translateVector = pos; }

	XMFLOAT3 _translateVector;
	XMFLOAT3 _rotateAngles;
	XMFLOAT3 OldVector;
	//not really any need for scale

	XMFLOAT4X4 world;
	XMFLOAT4X4 scale;
	XMFLOAT4X4 rotate;
	XMFLOAT4X4 translate;

	MeshData* mesh;
	std::vector<Group*> _groups;//Since graphs arent needed within a model we'll just hold everything on the game level
	std::vector<int> _priorityGroups;
private:
	
};

struct SGnode{
	Model* _model;
	std::vector<SGnode> _children;
	bool _billBoard;
	SGnode(bool b){ 
		_billBoard = b;
	};
	SGnode(){ 
		_billBoard = false;
	};
};
