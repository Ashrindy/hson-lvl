#include "res-model.h"

using namespace ulvl::gfx;

void ResModel::Load(void* data, size_t size) {
	binaryData = data;

	ModelData& modelData = getModelData();

	model = new InstancedModel{};

	size_t vertCount{ 0 };
	size_t indicesCount{ 0 };
	for (auto& mesh : modelData.meshes) {
		vertCount += mesh.vertexCount;
		indicesCount += mesh.indexCount;
	}

	void* vertices{ new char[vertCount * sizeof(BaseVertex)]};
	std::vector<unsigned short> indices{};
	indices.resize(indicesCount);
	
	auto& mainVInfo = model->pipeline.getMainVertexBuffer().vertexInfo;
	mainVInfo.calcStrideByLayout();
	auto& mainLayout = mainVInfo.vertexLayout;
	auto layoutSize = mainVInfo.stride;

	size_t currentIdx{ 0 };
	for (auto& mesh : modelData.meshes) {
		char* cVerts = (char*)vertices;
		size_t vertSize = mesh.vertexCount * layoutSize;
		memcpy(&cVerts[currentIdx], VertexInfo::convertVertices(mesh.vertices, mesh.vertexCount, mesh.vertexInfo.vertexLayout, mainLayout), vertSize);
		currentIdx += vertSize;
	}

	currentIdx = 0;
	for (auto& mesh : modelData.meshes) {
		size_t idxSize = mesh.indexCount * sizeof(unsigned short);
		memcpy(&indices.data()[currentIdx], mesh.indices, idxSize);
		currentIdx += mesh.indexCount;
	}

	model->setModel(vertices, vertCount, indices.data(), indices.size());

	delete[] (char*)vertices;
}
