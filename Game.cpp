#pragma once
#include "Game.h"


Game::Game(){
	theApp = new Application();
}

Game::~Game(){

}

HRESULT Game::Initialise(HINSTANCE hInstance, int nCmdShow){
	HRESULT h = theApp->Initialise(hInstance, nCmdShow);
	if (FAILED(h)){
		return h;
	}
	
	_device = theApp->_pd3dDevice;
	_cameraManager = new CameraManager();
	_cameraManager->AddCamera("TopDown", CameraData(XMFLOAT4(0.0f, 3000.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI * 0.5f, 0.0f, 0.0f), theApp->_WindowWidth, theApp->_WindowHeight, 1.0f, 16000.0f));
	_cameraManager->AddCamera("Cinematic", CameraData(XMFLOAT4(-1800.0f, 500.0f, 0.0f, 0.0f), XMFLOAT3(XM_PI * 0.1f, XM_PI * 0.5f, 0.0f), theApp->_WindowWidth, theApp->_WindowHeight, 1.0f, 16000.0f));
	_cameraManager->AddCamera("Close", CameraData(XMFLOAT4(0.0f, 10.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), theApp->_WindowWidth, theApp->_WindowHeight, 1.0f, 16000.0f));
	_cameraManager->AddCamera("Reverse", CameraData(XMFLOAT4(0.0f, 10.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI, 0.0f), theApp->_WindowWidth, theApp->_WindowHeight, 1.0f, 16000.0f));
	_cameraManager->AddCamera("Free", CameraData(XMFLOAT4(0.0f, 20.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), theApp->_WindowWidth, theApp->_WindowHeight, 1.0f, 16000.0f));
	_activeCamera = _cameraManager->RetrieveCamera("TopDown");

	_modelDataManager = new ModelDataManager();
	mAICarController = new CarAIController();
	input = new Input();
	mIsInput = false;
	mPhysicsOff = false;
	mPhysicsNum = 0;
	return S_OK;
}

void Game::LoadContent(){
	//_modelDataManager->LoadTexture(_device, "NoTex.png");
	//_modelDataManager->LoadTexture(_device, "WhiteTex.png");
	_modelDataManager->LoadTexture(_device, "stone.dds");
	_modelDataManager->LoadTexture(_device, "floor.dds");
	_modelDataManager->Load("car.obj", _device); 
	_modelDataManager->Load("wheel.obj", _device); 
	_modelDataManager->Load("groundPlane.obj", _device); 
	_modelDataManager->Load("sky.obj", _device);
	_modelDataManager->Load("mountain.obj", _device);
	_modelDataManager->Load("tree1.obj", _device);
	_modelDataManager->Load("tree2.obj", _device);
	_modelDataManager->Load("tree3.obj", _device);
	_modelDataManager->Load("tree4.obj", _device);
	_modelDataManager->Load("grassTuft.obj", _device);
	_modelDataManager->Load("grassTuft2.obj", _device);
	std::ifstream inFile;
	inFile.open("SceneLayout.txt");

	if(!inFile.good())
	{
		std::cerr  << "ERROR: Cannot find Scene layout file\n";
		return;
	}
	else{
		std::cerr << "Scene layout File found\n";

		std::vector<SGnode*> graphPath;
		std::string input;
		while(!inFile.eof()){
			inFile >> input;
			if(input.compare("b") == 0){//base node
				std::string baseName;
				inFile >> baseName;
				_baseNode._model = _modelDataManager->ConstructModel(baseName);
				graphPath.push_back(&_baseNode);//first value should always be this
			}
			else if(input.compare("c") == 0){//new child
				std::string billBoard;
				inFile >> billBoard;
				if(billBoard.compare("t") == 0){
					graphPath.back()->_children.push_back(SGnode(true));
				}
				else{
					graphPath.back()->_children.push_back(SGnode(false));
				}
				std::string childName;
				inFile >> childName;
				graphPath.back()->_children.back()._model = _modelDataManager->ConstructModel(childName);
				float xp, yp, zp;
				inFile >> xp;
				inFile >> yp;
				inFile >> zp;
				graphPath.back()->_children.back()._model->_translateVector = XMFLOAT3(xp, yp, zp);
				graphPath.push_back(&graphPath.back()->_children.back());
			}
			else if(input.compare("u") == 0){//move up one level
				graphPath.pop_back();
			}
			else if (input.compare("O") == 0)
			{
				float x, y, z;
			
				inFile >> x;
				inFile >> y; 
				inFile >> z;
				mAICarController->AddWaypoint(XMFLOAT3(x, y, z));
			}
		}
	}

	skyBox = _modelDataManager->ConstructModel("sky.obj");
	myCar = &_baseNode._children[0];
	mCarParticleModel = new CarParticleModel(myCar);
	mCarBoundingShpere = new BSphere(&_baseNode._children[0]._model->_translateVector, 55.0f);
	mAIBoundingSphere = new BSphere(&_baseNode._children[1]._model->_translateVector, 55.0f);
	mAIAvoidBounding = new BSphere(&_baseNode._children[1]._model->_translateVector, 80.0f);
	mAICarController->SetAICar(&_baseNode._children[1]);
	mAICarController->SetPLayerCarPos(&_baseNode._children[0]._model->_translateVector);
	mInclinePlane = &_baseNode._children[1];
	mNormalParticleModel = new ParticleModel(mInclinePlane);
}

void Game::Update(float delta){
	input->Update();

	CameraController();

	if (input->IsKeyDownEx('U'))
	{
		cout << myCar->_model->_translateVector.x + myCar->_model->_translateVector.y << endl;
	}

	//Thats a hack if I ever saw one
	if(input->IsKeyDownEx(VK_TAB)){
		if(myCar == &_baseNode._children[0]){
			myCar = &_baseNode._children[1];
		}
		else if(myCar == &_baseNode._children[1]){
			myCar = &_baseNode._children[0];
		}
	}
	
	if (input->IsKeyDownEx('N'))
	{
		mPhysicsNum = 1;
		_baseNode._children[1]._model->_translateVector.y = 20.0f;
	}
	if (input->IsKeyDownEx('M'))
	{
		mPhysicsNum = 2;
		mNormalParticleModel->SetVelocity(0.0f, 5.0f, 0.0f);
		mNormalParticleModel->SetAcceleration(0.0f, 0.5f, 0.0f);
	}

	if (mPhysicsNum == 0)
	{
		mAICarController->Update(delta);
	}
	if (mPhysicsNum == 1)
	{
		mNormalParticleModel->SetPhysicsMode(PHYSICS_SLIDING);
		mNormalParticleModel->Update(delta);
	}
	if (mPhysicsNum == 2)
	{
		mNormalParticleModel->SetPhysicsMode(PHYSICS_MULTIFORCE);

		mNormalParticleModel->Update(delta);
	}
	//mCarParticleModel->Update(delta);

	mCarParticleModel->Update(delta);	
	VehicleController(myCar, delta);
	UpdateGraph(&_baseNode);
}

void Game::CollisionChecks(float delta)
{
	if (Collision::SphereToSphereCollision(mCarBoundingShpere, mAIBoundingSphere))
	{
		mCarParticleModel->CollisionResponse(*mAIBoundingSphere->GetCentre(), delta);
	}
	if (Collision::SphereToSphereCollision(mCarBoundingShpere, mAIAvoidBounding))
	{
		mAICarController->Avoid(*mCarBoundingShpere->GetCentre(), delta);
	}
}






void Game::CameraController(){
	if(input->IsKeyDownEx('1')){
		_activeCamera = _cameraManager->RetrieveCamera("TopDown");
	}
	else if(input->IsKeyDownEx('2')){
		_activeCamera = _cameraManager->RetrieveCamera("Cinematic");
	}
	else if(input->IsKeyDownEx('3')){
		_activeCamera = _cameraManager->RetrieveCamera("Close");
	}
	else if(input->IsKeyDownEx('4')){
		_activeCamera = _cameraManager->RetrieveCamera("Reverse");
	}
	else if(input->IsKeyDownEx('F')){
		XMFLOAT3 angles = _activeCamera->GetAngles();
		XMFLOAT4 pos = _activeCamera->GetEye();
		_activeCamera = _cameraManager->RetrieveCamera("Free");
		_activeCamera->SetAngles(angles);
		_activeCamera->SetEye(pos);
	}

	//Allow looking around
	if(_activeCamera->GetName() == "Close" ||
		_activeCamera->GetName() == "Reverse" ||
		_activeCamera->GetName() == "Free"){
		if(input->IsKeyDown(VK_LEFT)){
			_activeCamera->AppendAnglesY(-0.015f);
		}
		else if(input->IsKeyDown(VK_RIGHT)){
			_activeCamera->AppendAnglesY(0.015f);
		}
		if(input->IsKeyDown(VK_UP)){
			_activeCamera->AppendAnglesX(-0.015f);
		}
		else if(input->IsKeyDown(VK_DOWN)){
			_activeCamera->AppendAnglesX(0.015f);
		}

	}

	//Allow moving around
	if(_activeCamera->GetName() == "Free"){
		if(input->IsKeyDown('U')){
			_activeCamera->Forward(4.0f);
		}
		else if(input->IsKeyDown('J')){
			_activeCamera->Forward(-4.0f);
		}
		if(input->IsKeyDown('H')){
			_activeCamera->Strafe(4.0f);
		}
		else if(input->IsKeyDown('K')){
			_activeCamera->Strafe(-4.0f);
		}
		if(input->IsKeyDown('Y')){
			_activeCamera->Vertical(4.0f);
		}
		else if(input->IsKeyDown('I')){
			_activeCamera->Vertical(-4.0f);
		}
	}
	
	_activeCamera->Update();//Only bother updating the camera in use


	XMFLOAT2 carHeading;
	carHeading.x = sin(myCar->_model->_rotateAngles.y);
	carHeading.y = cos(myCar->_model->_rotateAngles.y);//y is applied to Z here

	XMFLOAT3 carPos = myCar->_model->_translateVector;
	XMFLOAT4 newEye(carPos.x, carPos.y, carPos.z, 0.0f);

	if(_activeCamera->GetName() == "Close"){
		newEye.x -= (carHeading.x * 75);
		newEye.y += 75;
		newEye.z -= (carHeading.y * 75);
		_activeCamera->SetEye(newEye);
		_activeCamera->SetAnglesY(myCar->_model->_rotateAngles.y);
	}
	else if(_activeCamera->GetName() == "Reverse"){
		newEye.x -= (carHeading.x * 75);
		newEye.y += 75;
		newEye.z -= (carHeading.y * 75);
		_activeCamera->SetEye(newEye);
		_activeCamera->SetAnglesY(myCar->_model->_rotateAngles.y + XM_PI);
	}
}

void Game::VehicleController(SGnode* v, float t){
	if(input->IsKeyDown('W')){
		mCarParticleModel->MoveFowards(t, 1.0f);
		for(int i = 0; i < v->_children.size(); i++){
			//v->_children[i]._model->_rotateAngles.x += 0.01f; // wheel speed default
			v->_children[i]._model->_rotateAngles.x += mCarParticleModel->CalcWheelSpeed(t); // wheel speed
		}
		mIsInput = true;	
	}
	else if(input->IsKeyDown('S')){
		mCarParticleModel->MoveFowards(t, -1.0f);
		for(int i = 0; i < v->_children.size(); i++){
			//v->_children[i]._model->_rotateAngles.x -= 0.003f; // wheel rotate speed
			v->_children[i]._model->_rotateAngles.x += mCarParticleModel->CalcWheelSpeed(t);
		}
		mIsInput = true;
	}
	if(input->IsKeyDown('A')){
		mCarParticleModel->Turn(t, -1.0f);
		for(int i = 2; i < v->_children.size(); i++){
			v->_children[i]._model->_rotateAngles.y = mCarParticleModel->GetCarVelocity().x * 25;
		}
	
	}
	else if(input->IsKeyDown('D')){
		mCarParticleModel->Turn(t, 1.0f);
		for(int i = 2; i < v->_children.size(); i++){
			v->_children[i]._model->_rotateAngles.y = mCarParticleModel->GetCarVelocity().x * 25;
		}
	
	}
	else{
		mCarParticleModel->Turn(t, 0.0f);
		for (int i = 2; i < v->_children.size(); i++){
			v->_children[i]._model->_rotateAngles.y = mCarParticleModel->GetCarVelocity().x * 25;
		}
	}

	if (mIsInput == false)
	{
		mCarParticleModel->MoveFowards(t, 0.0f);
		for (int i = 0; i < v->_children.size(); i++){
			//v->_children[i]._model->_rotateAngles.x += 0.01f; // wheel speed default
			v->_children[i]._model->_rotateAngles.x += mCarParticleModel->CalcWheelSpeed(t); // wheel speed
		}
	}


	if (input->IsKeyDown('U')) // Change Gear
	{
		mCarParticleModel->mCarAcceleration.z = 1.0;
		mCarParticleModel->mCarMaxSpeed.z = 8.0f;
	}
	if (input->IsKeyDown('I'))
	{
		mCarParticleModel->mCarAcceleration.z = 2.0;
		mCarParticleModel->mCarMaxSpeed.z = 10.0f;
	}
	if (input->IsKeyDown('O'))
	{
		mCarParticleModel->mCarAcceleration.z = 6.0;
		mCarParticleModel->mCarMaxSpeed.z = 15.0f;
	}

	CollisionChecks(t);
	//mCarParticleModel->Update(t);
	mIsInput = false;
}

void Game::UpdateGraph(SGnode* currentNode){
	if(currentNode->_billBoard){
		XMFLOAT2 camPos(_activeCamera->GetEye().x, _activeCamera->GetEye().z);
		XMFLOAT2 boardPos(currentNode->_model->_translateVector.x, currentNode->_model->_translateVector.z);
		float angle = atan2(camPos.x - boardPos.x, camPos.y - boardPos.y);
		currentNode->_model->_rotateAngles.y = angle;
	}

	currentNode->_model->Update();
	for(int i = 0; i < currentNode->_children.size(); i++){
		UpdateGraph(&currentNode->_children[i]);
	}
}

void Game::Draw(){
	theApp->Draw(&_baseNode, skyBox, _activeCamera);//hand in lights, models and the camera to the graphics engine to deal with
}
