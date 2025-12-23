#include "model-service.h"

using namespace ulvl::app;

MeshData::~MeshData() {
	/*if (vertices)
		delete vertices;

	if (indices)
		delete indices;*/
}

MeshData::VertSet& MeshData::getVertices(const std::vector<plume::RenderInputElement>& vertLayout) {
	for (auto& vertSet : vertsets) {
		if (gfx::VertexInfo::compareLayouts(vertSet.vertexInfo.vertexLayout, vertLayout))
			return vertSet;
	}

	auto& vertset = vertsets.emplace_back();
	vertset.vertexInfo.vertexLayout = vertLayout;
	vertset.vertexInfo.calcStrideByLayout();

	if (vertsets.size() > 1) {
		auto& baseVertSet = vertsets[0];
		vertset.vertices = gfx::VertexInfo::convertVertices(baseVertSet.vertices, vertexCount, baseVertSet.vertexInfo.vertexLayout, vertLayout);
	}
	
	return vertset;
}

ModelData* ModelService::getModel(unsigned int nameHash) {
	for (auto* mdl : modelDatas) {
		if (mdl->nameHash == nameHash)
			return mdl;
	}

	ModelData* mdlData{ new ModelData{} };
	mdlData->nameHash = nameHash;
	modelDatas.push_back(mdlData);
	return mdlData;
}
