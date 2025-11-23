#include "instancedmodel.h"
#include "graphics.h"
#include "shaders/instance_vs.h"
#include "shaders/color_ps.h"

using namespace ulvl::gfx;

InstancedModel::InstancedModel(ModelDesc desc) : BaseModel{} {
    init(desc);
}

void InstancedModel::updateInstanceBuffer() {
    auto* graphics = Graphics::instance;
    auto& ctx = graphics->renderCtx;

    if (meshes.size() > 0)
        pipeline.setVertices(meshes.data(), meshes.size(), 1);
}

void InstancedModel::setModel(void* vertices, unsigned int vcount, unsigned short* indices, unsigned int icount) {
    auto* gfx = Graphics::instance;
    auto& ctx = gfx->renderCtx;

    pipeline.setVertices(vertices, vcount, 0);
    pipeline.setIndices(indices, icount);
}

void InstancedModel::addMesh(InstancedMesh mesh) {
    meshes.push_back(mesh);
    updateInstanceBuffer();
}

void InstancedModel::removeMeshes(int id) {
    std::vector<InstancedMesh> meshesToDelete{};
    for (auto& mesh : meshes)
        if (mesh.id == id)
            meshesToDelete.push_back(mesh);

    for (auto& mesh : meshesToDelete)
        meshes.erase(std::remove(meshes.begin(), meshes.end(), mesh));
}

void InstancedModel::clearMeshes() {
    if (meshes.size() == 0) return;

    meshes.clear();
    updateInstanceBuffer();
}

void InstancedModel::init(ModelDesc desc) {
    auto* graphics = Graphics::instance;
    auto& ctx = graphics->renderCtx;

    if (!desc.vertexShader) {
        desc.vertexShader = instance_vs_shader;
        desc.vertexShaderSize = sizeof(instance_vs_shader);
    }

    if (!desc.pixelShader) {
        desc.pixelShader = color_ps_shader;
        desc.pixelShaderSize = sizeof(color_ps_shader);
    }

    plume::RenderDescriptorRange range{};
    range.type = plume::RenderDescriptorRangeType::CONSTANT_BUFFER;
    range.binding = 0;
    range.count = 1;

    plume::RenderDescriptorSetDesc descriptorDesc{};
    descriptorDesc.descriptorRanges = &range;
    descriptorDesc.descriptorRangesCount = 1;

    Pipeline::Desc pipelineDesc{
        .vertexShader = desc.vertexShader,
        .vertexShaderSize = desc.vertexShaderSize,
        .pixelShader = desc.pixelShader,
        .pixelShaderSize = desc.pixelShaderSize,
        .primitiveTopo = desc.primitiveTopo,
        .cullMode = desc.cullMode,
        .descriptorSetDescs { descriptorDesc },
        .vertexBufferDescs = {
            { .vertexLayout = desc.vertexLayout },
            {
                .vertexLayout = {
                    { "MAT41",    0, 2, plume::RenderFormat::R32G32B32A32_FLOAT, 1, 0                  },
                    { "MAT42",    0, 3, plume::RenderFormat::R32G32B32A32_FLOAT, 1, sizeof(float) * 4  },
                    { "MAT43",    0, 4, plume::RenderFormat::R32G32B32A32_FLOAT, 1, sizeof(float) * 8  },
                    { "MAT44",    0, 5, plume::RenderFormat::R32G32B32A32_FLOAT, 1, sizeof(float) * 12 },
                    { "COLOR",    0, 6, plume::RenderFormat::R32G32B32A32_FLOAT, 1, sizeof(float) * 16 },
                },
                .slotClass = plume::RenderInputSlotClassification::PER_INSTANCE_DATA,
                .stride = sizeof(InstancedMesh)
            }
        }
    };

    pipeline.init(pipelineDesc);
    pipeline.descriptors[0].buffers.emplace_back(ctx.mainCBuffer.get(), sizeof(MainCBuffer));
}

void InstancedModel::render() {
    if (!visible && meshes.size() == 0) return;

    auto* graphics = Graphics::instance;
    auto& ctx = graphics->renderCtx;

    pipeline.render();

    ctx.commandList->drawIndexedInstanced(pipeline.indexBuffer.indices.size(), meshes.size(), 0, 0, 0);
}

void InstancedModel::shutdown() {
    BaseModel::shutdown();
    pipeline.shutdown();
}

InstancedMesh::InstancedMesh() : worldMatrix{ 1 }, color{ 0, 0, 0, 1 } {}

void InstancedMesh::setPosition(const glm::vec3& pos) {
    worldMatrix[3][0] = pos.x;
    worldMatrix[3][1] = pos.y;
    worldMatrix[3][2] = pos.z;
}
