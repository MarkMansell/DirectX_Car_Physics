#include "Group.h"

Group::Group(std::string name, bool transparent, UINT numIndices, UINT indexStart, int vertexStart, Material* m, ID3D11ShaderResourceView* dif, ID3D11ShaderResourceView * spec){
	_name = name;
	_indexStart = indexStart;
	_numIndices = numIndices;
	_vertexStart = vertexStart;
	mat = m;
	diffuseTex = dif;
	specularTex = spec;

	_transparent = transparent;
}

Group::~Group(){

}

void Group::Draw(ID3D11DeviceContext* context, MeshData* mesh){// add start point parameters for the index array so we can draw inividual sub-shapes
	context->IASetVertexBuffers(0, 1, &mesh->VertexBuffer, &mesh->VBStride, &mesh->VBOffset);
	context->IASetIndexBuffer(mesh->IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	context->PSSetShaderResources(0, 1, &diffuseTex);
	context->PSSetShaderResources(1, 1, &specularTex);
	//context->DrawIndexed(mesh->IndexCount, 0, 0);
	context->DrawIndexed(_numIndices, _indexStart, 0);
}