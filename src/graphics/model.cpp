#include "model.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "graphics.h"

using namespace ulvl::gfx;

void Model::updateWorldMatrix() {
	worldMatrix =
		glm::translate(glm::mat4(1), position) *
		glm::toMat4(rotation) *
		glm::scale(glm::mat4(1), scale);
}

Model::Model(ModelDesc desc) : BaseModel{} {
    init(desc);
}

void Model::init(ModelDesc desc) {
    BaseModel::init(desc);
    pipeline.pushConstants[0].setData(&worldMatrix);
    updateWorldMatrix();
}

void Model::shutdown() {
    BaseModel::shutdown();
    meshes.clear();
}
 
void Model::setPosition(const glm::vec3& pos) {
	position = pos;
	updateWorldMatrix();
}

void Model::setRotation(const glm::quat& rot) {
	rotation = rot;
	updateWorldMatrix();
}

void Model::setRotation(const glm::vec3& rot) {
	setRotation(glm::quat(rot));
}

void Model::setScale(const glm::vec3& scale) {
	this->scale = scale;
	updateWorldMatrix();
}

void Model::setWorldMatrix(const glm::mat4& mat) {
    worldMatrix = mat;
    glm::vec3 skew{};
    glm::vec4 perspective{};
    glm::decompose(worldMatrix, scale, rotation, position, skew, perspective);
}

void Model::addMesh(void* vertices, unsigned int vcount, unsigned short* indices, unsigned int icount) {
    auto* gfx = Graphics::instance;
    auto& ctx = gfx->renderCtx;

    int indexOffset = pipeline.indexBuffer.indices.size();
    unsigned int indexC{ icount };
    if (!indices)
        indexC = vcount;
	meshes.emplace_back(indexOffset, indexC);

    pipeline.addVertices(vertices, vcount, 0);
    if (indices)
        pipeline.addIndices(indices, icount);
}

void Model::clearMeshes() {
    meshes.clear();
    pipeline.setVertices(nullptr, 0, 0);
    pipeline.setIndices(nullptr, 0);
}

void Model::render() {
    if (!visible && meshes.size() == 0) return;

    auto* graphics = Graphics::instance;
    auto& ctx = graphics->renderCtx;

    pipeline.render();

    if (pipeline.indexBuffer.buffer)
        for (auto& mesh : meshes)
            ctx.commandList->drawIndexedInstanced(mesh.indexCount, 1, mesh.indexOffset, 0, 0);
    else
        for (auto& mesh : meshes)
            ctx.commandList->drawInstanced(mesh.indexCount, 1, mesh.indexOffset, 0);
}
