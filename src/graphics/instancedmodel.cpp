#include "instancedmodel.h"
#include "graphics.h"
#include "shaders/instance_vs.h"
#include "shaders/color_ps.h"

using namespace ulvl::gfx;

InstancedModel::InstancedModel(ModelDesc desc) : BaseModel{ desc } {
    init(desc);
}

void InstancedModel::updateInstanceBuffer() {
    auto* graphics = Graphics::instance;
    auto& ctx = graphics->renderCtx;

    if (meshes.size() > 0) {
        unsigned int meshSize{ (unsigned int)meshes.size() * sizeof(InstancedMesh) };

        instanceBuffer = ctx.device->createBuffer(plume::RenderBufferDesc::VertexBuffer(meshSize, plume::RenderHeapType::UPLOAD));
        void* bufferData = instanceBuffer->map();
        std::memcpy(bufferData, meshes.data(), meshSize);
        instanceBuffer->unmap();

        vertexBufferView[1] = { plume::RenderBufferReference{ instanceBuffer.get() }, meshSize };
    }
}

void InstancedModel::setModel(void* vertices, unsigned int vcount, unsigned short* indices, unsigned int icount) {
    auto* graphics = Graphics::instance;
    auto& ctx = graphics->renderCtx;

    unsigned int verticesSize = vcount * vertexStride;
    unsigned int indicesSize = icount * sizeof(unsigned short);

    vertexCount = vcount;

    if (this->vertices)
        delete[] this->vertices;

    this->vertices = new char[verticesSize];
    memcpy(this->vertices, vertices, verticesSize);

    this->indices.clear();
    this->indices.resize(icount);
    memcpy(this->indices.data(), indices, indicesSize);

    vertexBuffer = ctx.device->createBuffer(plume::RenderBufferDesc::VertexBuffer(verticesSize, plume::RenderHeapType::UPLOAD));
    void* bufferData = vertexBuffer->map();
    memcpy(bufferData, this->vertices, verticesSize);
    vertexBuffer->unmap();

    vertexBufferView[0] = { plume::RenderBufferReference{ vertexBuffer.get() }, verticesSize };

    indexBuffer = ctx.device->createBuffer(plume::RenderBufferDesc::IndexBuffer(indicesSize, plume::RenderHeapType::UPLOAD));
    bufferData = indexBuffer->map();
    memcpy(bufferData, this->indices.data(), indicesSize);
    indexBuffer->unmap();

    indexBufferView = plume::RenderIndexBufferView{ { indexBuffer.get() }, indicesSize, plume::RenderFormat::R16_UINT };
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
    vertexStride = sizeof(PosVertex);

    auto* graphics = Graphics::instance;
    auto& ctx = graphics->renderCtx;

    plume::RenderDescriptorRange range{};
    range.type = plume::RenderDescriptorRangeType::CONSTANT_BUFFER;
    range.binding = 0;
    range.count = 1;

    plume::RenderDescriptorSetDesc descriptorDesc{};
    descriptorDesc.descriptorRanges = &range;
    descriptorDesc.descriptorRangesCount = 1;

    descriptor = ctx.device->createDescriptorSet(descriptorDesc);

    plume::RenderPipelineLayoutDesc layoutDesc{};
    layoutDesc.allowInputLayout = true;
    layoutDesc.descriptorSetDescs = &descriptorDesc;
    layoutDesc.descriptorSetDescsCount = 1;

    pipelineLayout = ctx.device->createPipelineLayout(layoutDesc);

    plume::RenderShaderFormat shaderFormat = ctx.renderInterface->getCapabilities().shaderFormat;

    std::unique_ptr<plume::RenderShader> vertexShader;
    std::unique_ptr<plume::RenderShader> fragmentShader;

    switch (shaderFormat) {
    case plume::RenderShaderFormat::SPIRV:
        vertexShader = ctx.device->createShader(instance_vs_shader, sizeof(instance_vs_shader), "main", shaderFormat);
        fragmentShader = ctx.device->createShader(color_ps_shader, sizeof(color_ps_shader), "main", shaderFormat);
        break;
    default:
        assert(false && "Unknown shader format");
    }

    inputSlots.emplace_back(0, vertexStride);
    inputSlots.emplace_back(1, sizeof(InstancedMesh), plume::RenderInputSlotClassification::PER_INSTANCE_DATA);

    vertexLayout = {
        { "POSITION", 0, 0, plume::RenderFormat::R32G32B32_FLOAT,    0, 0                  },

        { "MAT41",    0, 2, plume::RenderFormat::R32G32B32A32_FLOAT, 1, 0                  },
        { "MAT42",    0, 3, plume::RenderFormat::R32G32B32A32_FLOAT, 1, sizeof(float) * 4  },
        { "MAT43",    0, 4, plume::RenderFormat::R32G32B32A32_FLOAT, 1, sizeof(float) * 8  },
        { "MAT44",    0, 5, plume::RenderFormat::R32G32B32A32_FLOAT, 1, sizeof(float) * 12 },
        { "COLOR",    0, 6, plume::RenderFormat::R32G32B32A32_FLOAT, 1, sizeof(float) * 16 },
    };

    plume::RenderGraphicsPipelineDesc pipelineDesc{};
    pipelineDesc.inputSlots = inputSlots.data();
    pipelineDesc.inputSlotsCount = inputSlots.size();
    pipelineDesc.inputElements = vertexLayout.data();
    pipelineDesc.inputElementsCount = static_cast<uint32_t>(vertexLayout.size());
    pipelineDesc.pipelineLayout = pipelineLayout.get();
    pipelineDesc.vertexShader = vertexShader.get();
    pipelineDesc.pixelShader = fragmentShader.get();
    pipelineDesc.renderTargetFormat[0] = plume::RenderFormat::B8G8R8A8_UNORM;
    pipelineDesc.renderTargetBlend[0] = plume::RenderBlendDesc::AlphaBlend();
    pipelineDesc.renderTargetCount = 1;
    pipelineDesc.primitiveTopology = desc.primitiveTopo;
    pipelineDesc.cullMode = plume::RenderCullMode::BACK;

    pipelineDesc.depthEnabled = true;
    pipelineDesc.depthFunction = plume::RenderComparisonFunction::LESS;
    pipelineDesc.depthTargetFormat = plume::RenderFormat::D32_FLOAT;

    pipeline = ctx.device->createGraphicsPipeline(pipelineDesc);

    vertexBufferView.resize(2);
}

void InstancedModel::render() {
    if (!visible && meshes.size() == 0) return;

    auto* graphics = Graphics::instance;
    auto& ctx = graphics->renderCtx;

    ctx.commandList->setGraphicsPipelineLayout(pipelineLayout.get());
    ctx.commandList->setPipeline(pipeline.get());
    ctx.commandList->setVertexBuffers(0, vertexBufferView.data(), vertexBufferView.size(), inputSlots.data());
    ctx.commandList->setIndexBuffer(&indexBufferView);

    descriptor->setBuffer(0, ctx.mainCBuffer.get(), sizeof(MainCBuffer));
    ctx.commandList->setGraphicsDescriptorSet(descriptor.get(), 0);

    ctx.commandList->drawIndexedInstanced(indices.size(), meshes.size(), 0, 0, 0);
}

void InstancedModel::shutdown() {
    BaseModel::shutdown();
    instanceBuffer.reset();
}

InstancedMesh::InstancedMesh() : worldMatrix{ 1 }, color{ 0, 0, 0, 1 } {}

void InstancedMesh::setPosition(const glm::vec3& pos) {
    worldMatrix[3][0] = pos.x;
    worldMatrix[3][1] = pos.y;
    worldMatrix[3][2] = pos.z;
}
