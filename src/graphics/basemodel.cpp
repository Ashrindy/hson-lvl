#include "basemodel.h"
#include "graphics.h"
#include "shaders/vs.h"
#include "shaders/ps_color.h"

using namespace ulvl::gfx;

BaseModel::BaseModel(ModelDesc desc) {
    init(desc);
}

void BaseModel::init(ModelDesc desc) {
    auto* graphics = Graphics::instance;
    auto& ctx = graphics->renderCtx;
    vertexStride = desc.vertexStride;

    plume::RenderDescriptorRange range{};
    range.type = plume::RenderDescriptorRangeType::CONSTANT_BUFFER;
    range.binding = 0;
    range.count = 1;

    plume::RenderDescriptorSetDesc descriptorDesc{};
    descriptorDesc.descriptorRanges = &range;
    descriptorDesc.descriptorRangesCount = 1;

    descriptor = ctx.device->createDescriptorSet(descriptorDesc);

    plume::RenderPushConstantRange pushConstantRange{};
    pushConstantRange.binding = 0;
    pushConstantRange.set = 0;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(glm::mat4);
    pushConstantRange.stageFlags = plume::RenderShaderStageFlag::VERTEX;

    plume::RenderPipelineLayoutDesc layoutDesc{};
    layoutDesc.allowInputLayout = true;
    layoutDesc.descriptorSetDescs = &descriptorDesc;
    layoutDesc.descriptorSetDescsCount = 1;
    layoutDesc.pushConstantRanges = &pushConstantRange;
    layoutDesc.pushConstantRangesCount = 1;

    pipelineLayout = ctx.device->createPipelineLayout(layoutDesc);

    plume::RenderShaderFormat shaderFormat = ctx.renderInterface->getCapabilities().shaderFormat;

    std::unique_ptr<plume::RenderShader> vertexShader;
    std::unique_ptr<plume::RenderShader> fragmentShader;

    switch (shaderFormat) {
    case plume::RenderShaderFormat::SPIRV:
        if (desc.vertexShader)
            vertexShader = ctx.device->createShader(desc.vertexShader, desc.vertexShaderSize, "main", shaderFormat);
        else
            vertexShader = ctx.device->createShader(vs_shader, sizeof(vs_shader), "main", shaderFormat);
        if (desc.pixelShader)
            fragmentShader = ctx.device->createShader(desc.pixelShader, desc.pixelShaderSize, "main", shaderFormat);
        else
            fragmentShader = ctx.device->createShader(ps_color_shader, sizeof(ps_color_shader), "main", shaderFormat);
        break;
    default:
        assert(false && "Unknown shader format");
    }

    inputSlots.emplace_back(0, vertexStride);

    vertexLayout = desc.vertexLayout;

    plume::RenderGraphicsPipelineDesc pipelineDesc{};
    pipelineDesc.inputSlots = inputSlots.data();
    pipelineDesc.inputSlotsCount = inputSlots.size();
    pipelineDesc.inputElements = vertexLayout.data();
    pipelineDesc.inputElementsCount = static_cast<uint32_t>(vertexLayout.size());
    pipelineDesc.pipelineLayout = pipelineLayout.get();
    pipelineDesc.vertexShader = vertexShader.get();
    pipelineDesc.pixelShader = fragmentShader.get();
    pipelineDesc.renderTargetFormat[0] = plume::RenderFormat::B8G8R8A8_UNORM;
    pipelineDesc.renderTargetBlend[0] = plume::RenderBlendDesc::Copy();
    pipelineDesc.renderTargetCount = 1;
    pipelineDesc.primitiveTopology = desc.primitiveTopo;
    pipelineDesc.depthEnabled = true;
    pipelineDesc.depthFunction = plume::RenderComparisonFunction::LESS;
    pipelineDesc.depthWriteEnabled = true;
    pipelineDesc.depthTargetFormat = plume::RenderFormat::D32_FLOAT;

    pipeline = ctx.device->createGraphicsPipeline(pipelineDesc);
}

void BaseModel::shutdown() {
    delete[] vertices;
    vertices = nullptr;
    vertexLayout.clear();
    indices.clear();
    pipeline.reset();
    descriptor.reset();
    pipelineLayout.reset();
    vertexBuffer.reset();
    indexBuffer.reset();
}

size_t BaseModel::getVertexLayoutOffset(const char* semanticName) const {
    for (auto& l : vertexLayout)
        if (strcmp(l.semanticName, semanticName) == 0)
            return l.alignedByteOffset;

    return 0;
}
