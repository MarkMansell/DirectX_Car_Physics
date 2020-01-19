#pragma once
#include "Model.h"
#include <fstream>
#include <istream>
#include <sstream>
#include "ModelDataManager.h"
#include "Graphics.h"
#include "CameraManager.h"
#include "Input.h"
#include <math.h>
#include "CarParticleModel.h"
#include "CarAIController.h"
using namespace DirectX;

//Tree images from http://pngimg.com/img/nature/tree
//Grass tuft images from http://pngimg.com/img/nature/grass
//Flat grass and dirt images from http://seamless-pixels.blogspot.co.uk/p/free-seamless-ground-textures.html
//Skybox image from http://lfgrafix.com/grafix/C3DB-Skies/Skies_Free/SkyBasic-1.html

class Game{
public:
	Game();
	~Game();
	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);
	void LoadContent();
	void Update(float delta);
	void UpdateGraph(SGnode* currentNode);
	void CameraController();
	void VehicleController(SGnode* v, float delta);
	void Draw();
	void CollisionChecks(float delta);
	void CollisionResponse();

	Application * theApp;

	Input* input;

	SGnode _baseNode;
private:
	ID3D11Device* _device;//required for model loading
	
	ModelDataManager* _modelDataManager;
	BSphere* mCarBoundingShpere;
	BSphere* mAIBoundingSphere;
	BSphere* mAIAvoidBounding;
	CameraManager* _cameraManager;
	Camera* _activeCamera;
	SGnode* myCar;//For ease of access
	SGnode* mInclinePlane;
	SGnode* mParticleSystem;
	CarParticleModel* mCarParticleModel;
	CarAIController* mAICarController;
	ParticleModel* mNormalParticleModel;
	Model* _effectModel;
	Model* skyBox;
	bool mIsInput;
	bool mPhysicsOff;
	int mPhysicsNum;
};