#include "ModelDataManager.h"

ModelDataManager::ModelDataManager(){
}

ModelDataManager::~ModelDataManager(){

}

//READ .OBJ METHOD HERE
//
//Load in a .OBJ file, and add the meshes, materials, and textures to their appropriate maps.
//At the same time, construct a ModelDefinition to be used by the game to define a model with the scene graph dictated by that .OBJ file.

//Make this private, only to be used by the READOBJ function
void ModelDataManager::LoadTexture(ID3D11Device* device, std::string fileName){
	if(_textures.find(fileName) == _textures.end()){
		//It's not there, so load it
		ID3D11ShaderResourceView* tex;
		std::string filePath = ".\\Textures\\";
		filePath.append(fileName);
		//D3DX11CreateShaderResourceViewFromFile(device, std::wstring(filePath.begin(), filePath.end()).c_str(), 0, 0, &tex, 0);
		CreateDDSTextureFromFile(device, std::wstring(filePath.begin(), filePath.end()).c_str(),nullptr, &tex);
		//CreateDDSTextureFromFile(_pd3dDevice, L"Resources\\stone.dds", nullptr, &_pTextureRV);
		_textures[fileName] = tex;
	}
}

ID3D11ShaderResourceView* ModelDataManager::RetrieveTexture(std::string textureName){
	if(_textures.find(textureName) != _textures.end()){
		return _textures[textureName];
	}
	else{
		return nullptr;
	}
}

MeshData* ModelDataManager::RetrieveMesh(std::string meshName){
	if(_meshes.find(meshName) != _meshes.end()){
		return _meshes[meshName];
	}
	else{
		return nullptr;
	}
}

Material* ModelDataManager::RetrieveMaterial(std::string materialName){
	if(_materials.find(materialName) != _materials.end()){
		return _materials[materialName];
	}
	else{
		return nullptr;
	}
}

Model* ModelDataManager::ConstructModel(const std::string meshName){
	Model* m =  new Model(RetrieveMesh(meshName));
	std::vector<int> _transparentGroups;//Indices of groups, PRIORITY 1
	std::vector<int> _opaqueGroups;//Indices of groups, PRIORITY 2
	for (int i = 0; i < _meshSubObjs[meshName].size(); i++){
		Material* mat = RetrieveMaterial(_meshSubObjs[meshName][i].useMtl);
		ID3D11ShaderResourceView* diff = nullptr;
		ID3D11ShaderResourceView* spec = nullptr;
		if(mat != nullptr){
			diff = RetrieveTexture(mat->diffuseTexName);
			if(diff == nullptr){
				diff = RetrieveTexture("WhiteTex.dds");
			}
			spec = RetrieveTexture(mat->specularTexName);
			if(spec == nullptr){
				spec = RetrieveTexture("WhiteTex.dds");
			}
		}

		bool trans = mat->transparency < 1.0f ? true : false;
		//if(mat != nullptr){
		m->_groups.push_back(new Group(_meshSubObjs[meshName][i].id, trans, _meshSubObjs[meshName][i]._indexEndLocation - _meshSubObjs[meshName][i]._indexStartLocation, _meshSubObjs[meshName][i]._indexStartLocation, _meshSubObjs[meshName][i]._vertexStartLocation, mat, diff, spec));
		//}
		//else{
		//	m->_groups.push_back(new Group(subObjs[i]._fStart, subObjs[i]._fEnd));
		//}

		if(trans){
			_transparentGroups.push_back(i);
		}
		else{
			_opaqueGroups.push_back(i);
		}
	}
for(int i = 0; i < _opaqueGroups.size(); i++){
		m->_priorityGroups.push_back(_opaqueGroups[i]);
	}
	for(int i = 0; i < _transparentGroups.size(); i++){
		m->_priorityGroups.push_back(_transparentGroups[i]);
	}
	
	return m;
}


void ModelDataManager::LoadMaterialLibrary(ID3D11Device* device, const std::string fileName){
	if(RetrieveMaterial(fileName) != nullptr){
		return;
	}

	std::ifstream inFile;
	inFile.open(".\\Meshes\\" + fileName);

	if(!inFile.good())
	{
		std::cerr  << "ERROR: Cannot find OBJ file '" << fileName  << "'\n";
		return;
	}
	else{
		std::cerr << "OBJ File '" << fileName  << "' found\n";

		std::string input;
		std::string currentMatName;
		while(!inFile.eof()){
			inFile >> input; //Get the next input from the file
			//getline(inFile, input);

			//Check what type of input it was, we are only interested in vertex positions, texture coordinates, normals and indices, nothing else
			if(input.compare("newmtl") == 0){
				inFile >> currentMatName;
				_materials[currentMatName] = new Material();
				_materials[currentMatName]->matName = currentMatName;
			}
			else if(input.compare("Ns") == 0){
				inFile >> _materials[currentMatName]->specularExponent.x;
			}
			else if(input.compare("Ni") == 0){//Reflective index, unused
				char dump[128];
				inFile.getline(dump, 128);
			}
			else if(input.compare("d") == 0){//Alpha, unused
				inFile >> _materials[currentMatName]->transparency;
			}
			else if(input.compare("Tr") == 0){//Transparency, unused
				char dump[128];
				inFile.getline(dump, 128);
			}
			else if(input.compare("Tf") == 0){//Transmission filter, unused
				char dump[128];
				inFile.getline(dump, 128);
			}
			else if(input.compare("illum") == 0){//Illumination, unused
				char dump[128];
				inFile.getline(dump, 128);
			}
			else if(input.compare("Ka") == 0){
				inFile >> _materials[currentMatName]->ambient.x;
				inFile >> _materials[currentMatName]->ambient.y;
				inFile >> _materials[currentMatName]->ambient.z;
			}
			else if(input.compare("Kd") == 0){
				inFile >> _materials[currentMatName]->diffuse.x;
				inFile >> _materials[currentMatName]->diffuse.y;
				inFile >> _materials[currentMatName]->diffuse.z;
			}
			else if(input.compare("Ks") == 0){
				inFile >> _materials[currentMatName]->specular.x;
				inFile >> _materials[currentMatName]->specular.y;
				inFile >> _materials[currentMatName]->specular.z;
			}
			else if(input.compare("Ke") == 0){
				char dump[128];
				inFile.getline(dump, 128);
			}
			else if(input.compare("map_Ka") == 0){
				char dump[128];
				inFile.getline(dump, 128);
			}
			else if(input.compare("map_Kd") == 0){
				std::string dTexFileName;
				inFile >> dTexFileName;
				LoadTexture(device, dTexFileName);
				_materials[currentMatName]->diffuseTexName = dTexFileName;
			}
			else if(input.compare("map_Ks") == 0){
				std::string sTexFileName;
				inFile >> sTexFileName;
				LoadTexture(device, sTexFileName);
				_materials[currentMatName]->specularTexName = sTexFileName;
			}
		}
	}

	std::cerr << "MTL File '" << fileName  << "' loaded\n";
}

bool ModelDataManager::FindSimilarVertex(const SimpleVertex& vertex, std::map<SimpleVertex, unsigned short>& vertToIndexMap, unsigned short& index)
{
	std::map<SimpleVertex, unsigned short>::iterator it = vertToIndexMap.find(vertex);
	if(it == vertToIndexMap.end())
	{
		return false;
	}
	else
	{
		index = it->second;
		return true;
	}
}

void ModelDataManager::CreateIndices(const std::vector<XMFLOAT3>& inVertices, const std::vector<XMFLOAT2>& inTexCoords, const std::vector<XMFLOAT3>& inNormals, std::vector<unsigned short>& outIndices, std::vector<XMFLOAT3>& outVertices, std::vector<XMFLOAT2>& outTexCoords, std::vector<XMFLOAT3>& outNormals)
{
	//Mapping from an already-existing SimpleVertex to its corresponding index
	std::map<SimpleVertex, unsigned short> vertToIndexMap;

	int numVertices = inVertices.size();
	for(int i = 0; i < numVertices; ++i) //For each vertex
	{
		SimpleVertex vertex = { inVertices[i], inNormals[i],  inTexCoords[i] }; 

		unsigned short index;
		bool found = FindSimilarVertex(vertex, vertToIndexMap, index); //See if a vertex already exists in the buffer that has the same attributes as this one
		if(found) //if found, re-use it's index for the index buffer
		{
			outIndices.push_back(index);
		}
		else //if not found, add it to the buffer
		{
			outVertices.push_back(vertex.Pos);
			outTexCoords.push_back(vertex.TexC);
			outNormals.push_back(vertex.Normal);

			unsigned short newIndex = (unsigned short)outVertices.size() - 1;
			outIndices.push_back(newIndex);

			//Add it to the map
			vertToIndexMap[vertex] = newIndex;
		}
	}
}

//WARNING: This code makes a big assumption -- that your models have texture coordinates AND normals which they should have anyway (else you can't do texturing and lighting!)
//If your .obj file has no lines beginning with "vt" or "vn", then you'll need to change the Export settings in your modelling software so that it exports the texture coordinates 
//and normals. If you still have no "vt" lines, you'll need to do some texture unwrapping, also known as UV unwrapping.
void ModelDataManager::Load(char* filename, ID3D11Device* _pd3dDevice, bool invertTexCoords)
{
	std::string filePath(filename);
	std::ifstream inFile;
	inFile.open(".\\Meshes\\" + filePath);

	if(!inFile.good())
	{
		std::cerr  << "ERROR: Cannot find OBJ file '" << filename  << "'\n";
		return;
	}
	else{
		std::cerr << "OBJ File '" << filename  << "' found\n";

		_meshSubObjs[filename];

		//Vectors to store the vertex positions, normals and texture coordinates. Need to use vectors since they're resizeable and we have
		//no way of knowing ahead of time how large these meshes will be
		std::vector<XMFLOAT3> verts;
		std::vector<XMFLOAT3> normals;
		std::vector<XMFLOAT2> texCoords;


		//DirectX uses 1 index buffer, OBJ is optimized for storage and not rendering and so uses 3 smaller index buffers.....great...
		//We'll have to merge this into 1 index buffer which we'll do after loading in all of the required data.
		std::vector<unsigned short> vertIndices;
		std::vector<unsigned short> normalIndices;
		std::vector<unsigned short> textureIndices;

		std::string input;
		int indexCounter = 0;

		while(!inFile.eof()) //While we have yet to reach the end of the file...
		{
			inFile >> input; //Get the next input from the file
			//getline(inFile, input);

			//Check what type of input it was, we are only interested in vertex positions, texture coordinates, normals and indices, nothing else
			if(input.compare("v") == 0) //Vertex position
			{
				XMFLOAT3 vert;

				inFile >> vert.x;
				inFile >> vert.y;
				inFile >> vert.z;

				verts.push_back(vert);
			}
			else if(input.compare("vt") == 0) //Texture coordinate
			{
				XMFLOAT2 texCoord;

				inFile >> texCoord.x;
				inFile >> texCoord.y;

				if(invertTexCoords) texCoord.y = 1.0f - texCoord.y;

				texCoords.push_back(texCoord);
			}
			else if(input.compare("vn") == 0) //Normal
			{
				XMFLOAT3 normal;

				inFile >> normal.x;
				inFile >> normal.y;
				inFile >> normal.z;

				normals.push_back(normal);
			}
			else if(input.compare("f") == 0) //Face
			{
				unsigned short vInd[3]; //indices for the vertex position
				unsigned short tInd[3]; //indices for the texture coordinate
				unsigned short nInd[3]; //indices for the normal

				for(int i = 0; i < 3; ++i)
				{
					inFile >> input;
					int slash = input.find("/"); //Find first forward slash
					int secondSlash = input.find("/", slash + 1); //Find second forward slash

					//Extract from string
					std::string beforeFirstSlash = input.substr(0, slash); //The vertex position index
					std::string afterFirstSlash = input.substr(slash + 1, secondSlash - slash - 1); //The texture coordinate index
					std::string afterSecondSlash = input.substr(secondSlash + 1); //The normal index

					//Parse into int
					vInd[i] = (unsigned short)atoi(beforeFirstSlash.c_str()); //atoi = "ASCII to int"
					tInd[i] = (unsigned short)atoi(afterFirstSlash.c_str());
					nInd[i] = (unsigned short)atoi(afterSecondSlash.c_str());
				}

				//sets fstart to an iterator, iterator position is 1 past the final element of vertindices (could use normals or textures, no matter).
				if(vertIndices.size() != 0){
					_meshSubObjs[filename].back()._vertexStartLocation =  vertIndices.back() < _meshSubObjs[filename].back()._vertexStartLocation ? vertIndices.back() : _meshSubObjs[filename].back()._vertexStartLocation;
				}
				_meshSubObjs[filename].back()._indexStartLocation =  indexCounter < _meshSubObjs[filename].back()._indexStartLocation ? indexCounter : _meshSubObjs[filename].back()._indexStartLocation;
				

				//Place into vectors
				for(int i = 0; i < 3; ++i)
				{
					vertIndices.push_back(vInd[i] - 1);		//Minus 1 from each as these as OBJ indexes start from 1 whereas C++ arrays start from 0
					textureIndices.push_back(tInd[i] - 1);	//which is really annoying. Apart from Lua and SQL, there's not much else that has indexing 
					normalIndices.push_back(nInd[i] - 1);	//starting at 1. So many more languages index from 0, the .OBJ people screwed up there.
					indexCounter++;
				}

				//sets fend to an iterator, iterator position is 1 past the final element of vertices (could use normals or textures, no matter).
				//As long as we use a less than operator this should work
				//aaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbccccccccccccccccc
				//..................---------------------------................
				//                  ^                         ^
				_meshSubObjs[filename].back()._indexEndLocation =  indexCounter > _meshSubObjs[filename].back()._indexEndLocation ? indexCounter : _meshSubObjs[filename].back()._indexEndLocation;
				_meshSubObjs[filename].back()._vertexEndLocation =  vertIndices.back() > _meshSubObjs[filename].back()._vertexEndLocation ? vertIndices.back() : _meshSubObjs[filename].back()._vertexEndLocation;

			}
			else if(input.compare("g") == 0)//Subobject name
			{
				_meshSubObjs[filename].push_back(SubObjData());
				inFile >> _meshSubObjs[filename].back().id;
				_meshSubObjs[filename].back()._vertexEndLocation = 0;
				_meshSubObjs[filename].back()._vertexStartLocation = UINT_MAX;
				_meshSubObjs[filename].back()._indexEndLocation = 0;
				_meshSubObjs[filename].back()._indexStartLocation = UINT_MAX;
			}
			else if(input.compare("mtllib") == 0)//Material library, calls readMaterial()
			{
				char matLibTmp[128];
				inFile >> matLibTmp;
				std::string matLibStr(matLibTmp);
				LoadMaterialLibrary(_pd3dDevice, matLibStr);
			}
			else if(input.compare("usemtl") == 0)//Use material, changes current set material for faces
			{
				inFile >> _meshSubObjs[filename].back().useMtl;
			}
			else if(input.compare("s") == 0)//Smoothing group
			{
				char dump[128];
				inFile.getline(dump, 128);
			}
		}
		inFile.close(); //Finished with input file now, all the data we need has now been loaded in

		//Get vectors to be of same size, ready for singular indexing
		std::vector<XMFLOAT3> expandedVertices;
		std::vector<XMFLOAT3> expandedNormals;
		std::vector<XMFLOAT2> expandedTexCoords;
		unsigned int numIndices = vertIndices.size();
		for(unsigned int i = 0; i < numIndices; i++)
		{
			expandedVertices.push_back(verts[vertIndices[i]]);
			expandedTexCoords.push_back(texCoords[textureIndices[i]]);
			expandedNormals.push_back(normals[normalIndices[i]]);
		}

		//Now to (finally) form the final vertex, texture coord, normal list and single index buffer using the above expanded vectors
		std::vector<unsigned short> meshIndices;
		std::vector<XMFLOAT3> meshVertices;
		std::vector<XMFLOAT3> meshNormals;
		std::vector<XMFLOAT2> meshTexCoords;
		CreateIndices(expandedVertices, expandedTexCoords, expandedNormals, meshIndices, meshVertices, meshTexCoords, meshNormals);

		MeshData* meshData = new MeshData();

		//Turn data from vector form to arrays
		SimpleVertex* finalVerts = new SimpleVertex[meshVertices.size()];
		unsigned int numMeshVertices = meshVertices.size();
		for(unsigned int i = 0; i < numMeshVertices; ++i)
		{
			finalVerts[i].Pos = meshVertices[i];
			finalVerts[i].Normal = meshNormals[i];
			finalVerts[i].TexC = meshTexCoords[i];
		}

		//Put data into vertex and index buffers, then pass the relevant data to the MeshData object.
		//The rest of the code will hopefully look familiar to you, as it's similar to whats in your InitVertexBuffer and InitIndexBuffer methods
		ID3D11Buffer* vertexBuffer;

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(SimpleVertex) * meshVertices.size();
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = finalVerts;

		_pd3dDevice->CreateBuffer(&bd, &InitData, &vertexBuffer);

		meshData->VertexBuffer = vertexBuffer;
		meshData->VBOffset = 0;
		meshData->VBStride = sizeof(SimpleVertex);

		unsigned short* indicesArray = new unsigned short[meshIndices.size()];
		unsigned int numMeshIndices = meshIndices.size();
		for(unsigned int i = 0; i < numMeshIndices; ++i)
		{
			indicesArray[i] = meshIndices[i];
		}

		ID3D11Buffer* indexBuffer;

		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(WORD) * meshIndices.size();     
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = indicesArray;
		_pd3dDevice->CreateBuffer(&bd, &InitData, &indexBuffer);

		meshData->IndexCount = meshIndices.size();
		meshData->IndexBuffer = indexBuffer;

		//This data has now been sent over to the GPU so we can delete this CPU-side stuff
		delete [] indicesArray;
		delete [] finalVerts;

		_meshes[filename] = meshData;
		std::cerr << "OBJ File '" << filename  << "' loaded\n";
		//return meshData;
	}	
}