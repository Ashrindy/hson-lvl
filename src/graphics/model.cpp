#include "model.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "graphics.h"
#include "shaders/vs.h"
#include "shaders/ps_color.h"

using namespace ulvl::gfx;

void Model::updateWorldMatrix() {
	worldMatrix =
		glm::translate(glm::mat4(1), position) *
		glm::toMat4(rotation) *
		glm::scale(glm::mat4(1), scale);
}

void Model::updateAabb() {
    size_t posOffset{ getVertexLayoutOffset("POSITION") };

    for (size_t idx = 0; idx < vertexCount; idx++) {
        auto& position = getVertexValue<glm::vec3>(idx, posOffset);
        aabb.min = glm::min(aabb.min, position);
        aabb.max = glm::max(aabb.max, position);
    }
}

Model::Model(ModelDesc desc) : BaseModel{ desc } {
    init(desc);
}

void Model::init(ModelDesc desc) {
    BaseModel::init(desc);
    
    updateWorldMatrix();
    updateAabb();
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

void Model::addMesh(void* vertices, unsigned int vcount, unsigned short* indices, unsigned int icount, void* texture) {
    auto* gfx = Graphics::instance;
    auto& ctx = gfx->renderCtx;

    int indexOffset = this->indices.size();
	int vertexOffset = vertexCount;
    unsigned int indexC{ icount };
    if (!indices)
        indexC = vcount;
	meshes.emplace_back(indexOffset, indexC, texture);

    char* newVerts = new char[(vertexCount + vcount) * vertexStride];
    if (this->vertices) {
        memcpy(newVerts, this->vertices, vertexCount * vertexStride);
        delete[] this->vertices;
    }
    this->vertices = newVerts;
    memcpy(&newVerts[vertexCount * vertexStride], vertices, vcount * vertexStride);
    
    vertexCount += vcount;

    if (indices) {
        this->indices.insert(this->indices.end(), indices, indices + icount);

        for (int x = 0; x < icount; x++) {
            auto& y = this->indices[indexOffset + x];
            y += vertexOffset;
        }
    }

    unsigned int verticesSize = vertexCount * vertexStride;
    vertexBuffer = ctx.device->createBuffer(plume::RenderBufferDesc::VertexBuffer(verticesSize, plume::RenderHeapType::UPLOAD));
    void* bufferData = vertexBuffer->map();
    memcpy(bufferData, this->vertices, verticesSize);
    vertexBuffer->unmap();

    vertexBufferView.clear();
    vertexBufferView.emplace_back(plume::RenderBufferReference{ vertexBuffer.get() }, verticesSize);

    if (indices) {
        unsigned int indicesSize = this->indices.size() * sizeof(unsigned short);
        indexBuffer = ctx.device->createBuffer(plume::RenderBufferDesc::IndexBuffer(indicesSize, plume::RenderHeapType::UPLOAD));
        bufferData = indexBuffer->map();
        memcpy(bufferData, this->indices.data(), indicesSize);
        indexBuffer->unmap();
        indexBufferView = plume::RenderIndexBufferView{ { indexBuffer.get() }, indicesSize, plume::RenderFormat::R16_UINT };
    }

    updateAabb();
}

void Model::clearMeshes() {
    meshes.clear();
    vertexCount = 0;
    delete[] vertices;
    vertices = nullptr;
    indices.clear();
}

void Model::render() {
    if (!visible && meshes.size() == 0) return;

    auto* graphics = Graphics::instance;
    auto& ctx = graphics->renderCtx;

    ctx.commandList->setGraphicsPipelineLayout(pipelineLayout.get());
    ctx.commandList->setPipeline(pipeline.get());
    ctx.commandList->setVertexBuffers(0, vertexBufferView.data(), vertexBufferView.size(), inputSlots.data());
    if (indexBuffer)
        ctx.commandList->setIndexBuffer(&indexBufferView);

    ctx.commandList->setGraphicsPushConstants(0, &worldMatrix, 0, sizeof(glm::mat4));

    descriptor->setBuffer(0, ctx.mainCBuffer.get(), sizeof(MainCBuffer));
    ctx.commandList->setGraphicsDescriptorSet(descriptor.get(), 0);

    if (indexBuffer) {
        for (auto& mesh : meshes) {
            ctx.commandList->drawIndexedInstanced(mesh.indexCount, 1, mesh.indexOffset, 0, 0);
        }
    }
    else {
        for (auto& mesh : meshes) {
            ctx.commandList->drawInstanced(mesh.indexCount, 1, mesh.indexOffset, 0);
        }
    }
}
