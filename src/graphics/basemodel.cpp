#include "basemodel.h"
#include "graphics.h"
#include "shaders/vs.h"
#include "shaders/ps.h"

using namespace ulvl::gfx;

BaseModel::BaseModel(ModelDesc desc) {
    init(desc);
}

void BaseModel::init(ModelDesc desc) {
    auto* graphics = Graphics::instance;
    auto& ctx = graphics->renderCtx;

    if (!desc.vertexShader) {
        desc.vertexShader = vs_shader;
        desc.vertexShaderSize = sizeof(vs_shader);
    }

    if (!desc.pixelShader) {
        desc.pixelShader = ps_shader;
        desc.pixelShaderSize = sizeof(ps_shader);
    }

    plume::RenderDescriptorRange range{};
    range.type = plume::RenderDescriptorRangeType::CONSTANT_BUFFER;
    range.binding = 0;
    range.count = 1;

    plume::RenderDescriptorSetDesc descriptorDesc{};
    descriptorDesc.descriptorRanges = &range;
    descriptorDesc.descriptorRangesCount = 1;

    plume::RenderPushConstantRange pushConstantRange{};
    pushConstantRange.binding = 0;
    pushConstantRange.set = 0;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(glm::mat4);
    pushConstantRange.stageFlags = plume::RenderShaderStageFlag::VERTEX;

    Pipeline::Desc pipelineDesc{
        .vertexShader = desc.vertexShader,
        .vertexShaderSize = desc.vertexShaderSize,
        .pixelShader = desc.pixelShader,
        .pixelShaderSize = desc.pixelShaderSize,
        .primitiveTopo = desc.primitiveTopo,
        .cullMode = desc.cullMode,
        .descriptorSetDescs = { descriptorDesc },
        .pushConstantDescs = { pushConstantRange },
        .vertexBufferDescs = { {.vertexLayout = desc.vertexLayout } }
    };

    pipeline.init(pipelineDesc);
    pipeline.descriptors[0].buffers.emplace_back(ctx.mainCBuffer.get(), sizeof(MainCBuffer));
}

void BaseModel::shutdown() {
    pipeline.shutdown();
}

size_t VertexInfo::calcStrideByLayout(const std::vector<plume::RenderInputElement>& layout) {
    if (layout.size() == 0) return 0;

    auto& lastElem = layout[layout.size() - 1];
    return lastElem.alignedByteOffset + plume::RenderFormatSize(lastElem.format);;
}

bool VertexInfo::compareLayouts(const std::vector<plume::RenderInputElement>& a, const std::vector<plume::RenderInputElement>& b) {
    if (a.size() > 0 && b.size() > 0) {
        if (a.size() != b.size())
            return false;
        else
            for (auto x = 0; x < b.size(); x++) {
                auto& elemA = b[x];
                auto& elemB = a[x];
                if (strcmp(elemA.semanticName, elemB.semanticName) != 0 ||
                    elemA.semanticIndex != elemB.semanticIndex ||
                    elemA.location != elemB.location ||
                    elemA.format != elemB.format ||
                    elemA.slotIndex != elemB.slotIndex ||
                    elemA.alignedByteOffset != elemB.alignedByteOffset)
                {
                    return false;
                }
            }
    }
    return true;
}

void* VertexInfo::getItem(const void* vertices, const int vertexIdx, const size_t vertexStride, const int offset) {
    return &((char*)vertices)[vertexIdx * vertexStride + offset];
}

void* VertexInfo::getItem(const void* vertices, const int vertexIdx, const std::vector<plume::RenderInputElement>& layout, const char* semanticName, const int semanticIndex) {
    size_t stride{ calcStrideByLayout(layout) };
    int offset{ -1 };

    for (auto& elem : layout) {
        if (strcmp(elem.semanticName, semanticName) == 0 && elem.semanticIndex == semanticIndex) {
            offset = elem.alignedByteOffset;
            break;
        }
    }

    if (offset == -1) return nullptr;

    return getItem(vertices, vertexIdx, stride, offset);
}

void* VertexInfo::convertVertices(const void* vertices, const int vcount, const std::vector<plume::RenderInputElement>& origLayout, const std::vector<plume::RenderInputElement>& newLayout) {
    size_t stride{ calcStrideByLayout(newLayout) };
    size_t vertSize{ vcount * stride };
    void* newVerts = new char[vertSize];
    memset(newVerts, 0, vertSize);

    for (auto x = 0; x < vcount; x++) {
        void* vertex = &((char*)newVerts)[x * stride];
        size_t vertexPos{ 0 };
        for (auto& elem : newLayout) {
            auto* value = getItem(vertices, x, origLayout, elem.semanticName, elem.semanticIndex);

            if (value)
                memcpy(&((char*)vertex)[vertexPos], value, plume::RenderFormatSize(elem.format));

            vertexPos += elem.alignedByteOffset;
        }
    }

    return newVerts;
}

void VertexInfo::calcStrideByLayout() {
    stride = calcStrideByLayout(vertexLayout);
}

std::unique_ptr<plume::RenderShader> Shader::getShader() {
    auto* graphics = Graphics::instance;
    auto& ctx = graphics->renderCtx;
    plume::RenderShaderFormat shaderFormat{ ctx.renderInterface->getCapabilities().shaderFormat };

    std::unique_ptr<plume::RenderShader> shader{};

    switch (shaderFormat) {
    case plume::RenderShaderFormat::SPIRV:
        shader = ctx.device->createShader(data, size, "main", shaderFormat);
        break;
    default:
        assert(false && "Unknown shader format");
    }

    return std::move(shader);
}

void Pipeline::init(Desc& desc) {
    auto* graphics = Graphics::instance;
    auto& ctx = graphics->renderCtx;

    description = desc;

    for (auto& desc : desc.descriptorSetDescs)
        descriptors.emplace_back(desc);

    for (auto& desc : desc.pushConstantDescs)
        pushConstants.emplace_back(desc.size);

    plume::RenderPipelineLayoutDesc pipelineLayoutDesc{};
    pipelineLayoutDesc.allowInputLayout = true;
    pipelineLayoutDesc.descriptorSetDescs = desc.descriptorSetDescs.data();
    pipelineLayoutDesc.descriptorSetDescsCount = desc.descriptorSetDescs.size();
    pipelineLayoutDesc.pushConstantRanges = desc.pushConstantDescs.data();
    pipelineLayoutDesc.pushConstantRangesCount = desc.pushConstantDescs.size();
    pipelineLayout = ctx.device->createPipelineLayout(pipelineLayoutDesc);

    vertexShader = { desc.vertexShader, desc.vertexShaderSize };
    pixelShader = { desc.pixelShader, desc.pixelShaderSize };
    
    std::vector<plume::RenderInputElement> vertexLayout{};

    for (auto x = 0; x < desc.vertexBufferDescs.size(); x++) {
        auto& vDesc = desc.vertexBufferDescs[x];
        vertexBuffers.emplace_back(vDesc.vertexLayout, vDesc.slotClass);

        auto& vBuffer = vertexBuffers[x];
        auto& vInfo = vBuffer.vertexInfo;
        
        if (vDesc.stride != -1)
            vInfo.stride = vDesc.stride;

        vBuffer.inputSlot = { (unsigned int)x, (unsigned int)vInfo.stride, vDesc.slotClass};
        inputSlots.push_back(vBuffer.inputSlot);
        
        vertexLayout.insert(vertexLayout.end(), vInfo.vertexLayout.begin(), vInfo.vertexLayout.end());
    }

    auto vertShader = vertexShader.getShader();
    auto pixShader = pixelShader.getShader();

    plume::RenderGraphicsPipelineDesc pipelineDesc{};
    pipelineDesc.inputSlots = inputSlots.data();
    pipelineDesc.inputSlotsCount = inputSlots.size();
    pipelineDesc.inputElements = vertexLayout.data();
    pipelineDesc.inputElementsCount = static_cast<uint32_t>(vertexLayout.size());
    pipelineDesc.pipelineLayout = pipelineLayout.get();
    pipelineDesc.vertexShader = vertShader.get();
    pipelineDesc.pixelShader = pixShader.get();
    pipelineDesc.renderTargetFormat[0] = plume::RenderFormat::B8G8R8A8_UNORM;
    pipelineDesc.renderTargetBlend[0] = plume::RenderBlendDesc::Copy();
    pipelineDesc.renderTargetCount = 1;
    pipelineDesc.cullMode = desc.cullMode;
    pipelineDesc.primitiveTopology = desc.primitiveTopo;
    pipelineDesc.depthEnabled = desc.depth.enabled;
    pipelineDesc.depthFunction = desc.depth.function;
    pipelineDesc.depthWriteEnabled = desc.depth.writeEnabled;
    pipelineDesc.depthTargetFormat = plume::RenderFormat::D32_FLOAT;

    pipeline = ctx.device->createGraphicsPipeline(pipelineDesc);
}

void Pipeline::setVertices(void* vertices, unsigned int count, unsigned int vertexBufferIndex) {
    if (vertexBuffers.size() < vertexBufferIndex || count == 0) return;

    vertexBuffers[vertexBufferIndex].setVertices(vertices, count);

    updateVertexBufferViews();
}

void Pipeline::addVertices(void* vertices, unsigned int count, unsigned int vertexBufferIndex) {
    if (vertexBuffers.size() < vertexBufferIndex || count == 0) return;

    vertexBuffers[vertexBufferIndex].addVertices(vertices, count);

    updateVertexBufferViews();
}

void Pipeline::updateVertexBufferViews() {
    vertexBufferViews.clear();

    for (auto& buffer : vertexBuffers)
        vertexBufferViews.push_back(buffer.bufferView);
}

void Pipeline::render() {
    auto* graphics = Graphics::instance;
    auto& ctx = graphics->renderCtx;

    ctx.commandList->setGraphicsPipelineLayout(pipelineLayout.get());
    ctx.commandList->setPipeline(pipeline.get());

    ctx.commandList->setVertexBuffers(0, vertexBufferViews.data(), vertexBufferViews.size(), inputSlots.data());
    if (indexBuffer.buffer) 
        ctx.commandList->setIndexBuffer(&indexBuffer.bufferView);

    for (auto& pushConstant : pushConstants)
        ctx.commandList->setGraphicsPushConstants(0, pushConstant.data, 0, pushConstant.size);

    for (auto& descriptor : descriptors) {
        for (auto x = 0; x < descriptor.buffers.size(); x++) {
            auto& buffer = descriptor.buffers[x];
            descriptor.descriptor->setBuffer(x, buffer.buffer, buffer.size);
        }
    }

    for (auto x = 0; x < descriptors.size(); x++) ctx.commandList->setGraphicsDescriptorSet(descriptors[x].descriptor.get(), x);
}

void Pipeline::shutdown() {
    for (auto& buff : vertexBuffers) {
        delete[] buff.vertices;
        buff.vertices = nullptr;
        buff.vertexInfo.vertexLayout.clear();
    }
    indexBuffer.indices.clear();
    pipeline.reset();
    for (auto& desc : descriptors)
        desc.descriptor.reset();
    pipelineLayout.reset();
    for (auto& buff : vertexBuffers)
        buff.buffer.reset();
    indexBuffer.buffer.reset();
}

void VertexBuffer::addVertices(void* newVertices, unsigned int count) {
    auto* graphics = Graphics::instance;
    auto& ctx = graphics->renderCtx;
    size_t stride{ vertexInfo.stride };
    char* newVerts = new char[(vertexCount + count) * stride];

    if (vertices) {
        memcpy(newVerts, vertices, vertexCount * stride);
        delete[] vertices;
    }
    vertices = newVerts;

    memcpy(&newVerts[vertexCount * stride], newVertices, count * stride);

    vertexCount += count;

    unsigned int verticesSize = vertexCount * stride;
    buffer = ctx.device->createBuffer(plume::RenderBufferDesc::VertexBuffer(verticesSize, plume::RenderHeapType::UPLOAD));
    void* bufferData = buffer->map();
    memcpy(bufferData, this->vertices, verticesSize);
    buffer->unmap();

    bufferView = { plume::RenderBufferReference{ buffer.get() }, verticesSize };
}

void VertexBuffer::setVertices(void* newVertices, unsigned int count) {
    if (vertices) {
        delete[] vertices;
        vertices = nullptr;
    }
    vertexCount = 0;

    if (newVertices)
        addVertices(newVertices, count);
}

void IndexBuffer::setIndices(unsigned short* newIndices, unsigned int count) {
    indices.clear();

    if (newIndices)
        addIndices(newIndices, count);
}

void IndexBuffer::addIndices(unsigned short* newIndices, unsigned int count) {
    auto* graphics = Graphics::instance;
    auto& ctx = graphics->renderCtx;

    indices.insert(indices.end(), newIndices, newIndices + count);

    unsigned int indicesSize = indices.size() * sizeof(unsigned short);
    buffer = ctx.device->createBuffer(plume::RenderBufferDesc::IndexBuffer(indicesSize, plume::RenderHeapType::UPLOAD));
    void* bufferData = buffer->map();
    memcpy(bufferData, indices.data(), indicesSize);
    buffer->unmap();

    bufferView = plume::RenderIndexBufferView{ { buffer.get() }, indicesSize, plume::RenderFormat::R16_UINT };
}

DescriptorSet::DescriptorSet(plume::RenderDescriptorSetDesc& desc) {
    auto* graphics = Graphics::instance;
    auto& ctx = graphics->renderCtx;

    descriptor = ctx.device->createDescriptorSet(desc);
}
