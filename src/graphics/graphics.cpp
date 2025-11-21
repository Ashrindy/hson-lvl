#include "graphics.h"
#include "../ui/toolbar.h"

#include "fonts/Inter.h"
//#include "fonts/NotoSansJP.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include <imgui_impl_plume.h>

#include <ImGuizmo.h>

#ifdef WIN32
#include <windows.h>
#endif

using namespace ulvl::gfx;

Graphics* Graphics::instance = nullptr;

namespace plume {
#ifdef WIN32
    extern std::unique_ptr<RenderInterface> CreateVulkanInterface();
#else
    extern std::unique_ptr<RenderInterface> CreateVulkanInterface(RenderWindow sdlWindow);
#endif
    extern std::unique_ptr<RenderInterface> CreateD3D12Interface();
}

void Graphics::createFramebuffers(){
    renderCtx.framebuffers.clear();

    for (unsigned int i = 0; i < renderCtx.swapChain->getTextureCount(); i++) {
        const plume::RenderTexture* colorAttachment = renderCtx.swapChain->getTexture(i);

        plume::RenderFramebufferDesc fbDesc;
        fbDesc.colorAttachments = &colorAttachment;
        fbDesc.colorAttachmentsCount = 1;
        fbDesc.depthAttachment = nullptr;

        auto framebuffer = renderCtx.device->createFramebuffer(fbDesc);
        renderCtx.framebuffers.push_back(std::move(framebuffer));
    }
}

void ulvl::gfx::Graphics::createScreenFramebuffers() {
    if (!renderCtx.screenTex.get()) {
        plume::RenderTextureDesc texDesc{};
        texDesc.dimension = plume::RenderTextureDimension::TEXTURE_2D;
        texDesc.width = screenWidth;
        texDesc.height = screenHeight;
        texDesc.arraySize = 1;
        texDesc.depth = 1;
        texDesc.mipLevels = 1;
        texDesc.format = plume::RenderFormat::B8G8R8A8_UNORM;
        texDesc.flags = plume::RenderTextureFlag::RENDER_TARGET;
        renderCtx.screenTex = renderCtx.device->createTexture(texDesc);
    }

    if (!renderCtx.screenDTex.get()) {
        plume::RenderTextureDesc dtexDesc{};
        dtexDesc.dimension = plume::RenderTextureDimension::TEXTURE_2D;
        dtexDesc.width = screenWidth;
        dtexDesc.height = screenHeight;
        dtexDesc.arraySize = 1;
        dtexDesc.depth = 1;
        dtexDesc.mipLevels = 1;
        dtexDesc.format = plume::RenderFormat::D32_FLOAT;
        dtexDesc.flags = plume::RenderTextureFlag::DEPTH_TARGET;
        renderCtx.screenDTex = renderCtx.device->createTexture(dtexDesc);
    }

    if (!renderCtx.screenDTexView.get()) {
        plume::RenderTextureViewDesc dtexViewDesc{ plume::RenderTextureViewDesc::Texture2D(plume::RenderFormat::D32_FLOAT) };
        dtexViewDesc.mipLevels = 1;
        renderCtx.screenDTexView = renderCtx.screenDTex->createTextureView(dtexViewDesc);
    }

    if (!renderCtx.screenFb.get()) {
        plume::RenderFramebufferDesc fbDesc{};
        const plume::RenderTexture* attachments = renderCtx.screenTex.get();
        fbDesc.colorAttachments = &attachments;
        fbDesc.colorAttachmentsCount = 1;
        fbDesc.depthAttachment = renderCtx.screenDTex.get();
        fbDesc.depthAttachmentView = renderCtx.screenDTexView.get();
        renderCtx.screenFb = renderCtx.device->createFramebuffer(fbDesc);
    }

    if (renderCtx.screenDs) {
        renderCtx.screenDs->setTexture(0, renderCtx.screenTex.get(), plume::RenderTextureLayout::COLOR_WRITE);
        return;
    }

    plume::RenderDescriptorRange dr{};
    dr.type = plume::RenderDescriptorRangeType::TEXTURE;
    dr.count = 1;

    plume::RenderDescriptorSetDesc dcDesc{};
    dcDesc.descriptorRanges = &dr;
    dcDesc.descriptorRangesCount = 1;
    renderCtx.screenDs = renderCtx.device->createDescriptorSet(dcDesc);

    renderCtx.screenDs->setTexture(0, renderCtx.screenTex.get(), plume::RenderTextureLayout::COLOR_WRITE);
}

void Graphics::resize(unsigned int width, unsigned int height) {
    this->width = width;
    this->height = height;

    if (renderCtx.swapChain) {
        renderCtx.framebuffers.clear();

        if (!renderCtx.swapChain->resize())
            LOG("resize: Failed to resize swapchain!")

        createFramebuffers();
    }
}

void Graphics::resizeScreen(unsigned int width, unsigned int height) {
    screenWidth = width;
    screenHeight = height;

    renderCtx.screenFb.reset();
    renderCtx.screenTex.reset();
    renderCtx.screenDTexView.reset();
    renderCtx.screenDTex.reset();
    createScreenFramebuffers();
    if (camera)
        camera->setAspectRatio((float)width / (float)height);
}

void Graphics::reposScreen(unsigned int x, unsigned int y) {
    screenPosX = x;
    screenPosY = y;
}

void Graphics::initializeRenderResources() {
    renderCtx.device = renderCtx.renderInterface->createDevice();
    renderCtx.commandQueue = renderCtx.device->createCommandQueue(plume::RenderCommandListType::DIRECT);
    renderCtx.fence = renderCtx.device->createCommandFence();
    renderCtx.swapChain = renderCtx.commandQueue->createSwapChain(renderCtx.renderWindow, 2, plume::RenderFormat::B8G8R8A8_UNORM, 2);
    renderCtx.swapChain->resize();
    renderCtx.commandList = renderCtx.commandQueue->createCommandList();
    renderCtx.acquireSemaphore = renderCtx.device->createCommandSemaphore();
    renderCtx.commandFence = renderCtx.device->createCommandFence();
    createFramebuffers();
    createScreenFramebuffers();

    plume::RenderBufferDesc cBufferDesc{};
    cBufferDesc.size = sizeof(MainCBuffer);
    cBufferDesc.flags = plume::RenderBufferFlag::CONSTANT;
    cBufferDesc.heapType = plume::RenderHeapType::UPLOAD;
    renderCtx.mainCBuffer = renderCtx.device->createBuffer(cBufferDesc);
}

void Graphics::setUnsaved(bool unsaved) {
    if (unsaved) {
        char buffer[0x50];
        snprintf(buffer, sizeof(buffer), "%s*", name);
        SDL_SetWindowTitle(window, buffer);
    }
    else
        SDL_SetWindowTitle(window, name);
}

float Graphics::getDpiScale(SDL_Window* window) {
#ifdef WIN32
	HWND hwnd = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
	if (hwnd) {
		auto dpi = GetDpiForWindow(hwnd);
		float scale = dpi / 96.0f;
		return scale;
	}
	return 1.0f;
#elif
	return 1.0f;
#endif
}

bool Graphics::init() {
    if (!SDL_Init(SDL_INIT_VIDEO))
        LOGB("SDL: Failed to init SDL!");

    window = SDL_CreateWindow(name, width, height, SDL_WINDOW_RESIZABLE);
    if (!window)
        LOGB("SDL: Failed to init SDL window!");

#ifdef WIN32
    SDL_PropertiesID props = SDL_GetWindowProperties(window);
    if (props == 0)
        LOGB("SDL: No window properties were loaded!");

    renderCtx.renderWindow = (HWND)SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    renderCtx.renderInterface = plume::CreateVulkanInterface();
#else
    renderCtx.renderWindow = window;
    renderCtx.renderInterface = plume::CreateVulkanInterface(renderCtx.renderWindow);
#endif

    initializeRenderResources();

    // imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForOther(window);
    ImGui_ImplPlume_InitInfo initInfo{
        .rInterface = renderCtx.renderInterface.get(),
        .device = renderCtx.device.get()
    };
    ImGui_ImplPlume_Init(initInfo);

    auto dpiScale = getDpiScale(window);    
    io.Fonts->AddFontFromMemoryCompressedTTF((void*)InterFont_compressed_data, InterFont_compressed_size, 14 * dpiScale);

    // TODO: move this elsewhere if any custom style is implemented
    ImGui::GetStyle().ScaleAllSizes(dpiScale);

    // TODO: Find better font, NotoSansJP is really big and doesn't support all needed characters

    /*ImWchar notosansRange[]{
        0x0020, 0x00FF, 0x3000, 0x30FF, 0x31F0, 0x31FF, 0xFF00, 0xFFEF, 0x4E00, 0x9FAF
    };
    ImFontConfig cfg{};
    cfg.MergeMode = true;
    cfg.PixelSnapH = true;
    cfg.RasterizerMultiply = 5;
    io.Fonts->AddFontFromMemoryCompressedTTF((void*)NotoSansJPFont_compressed_data, NotoSansJPFont_compressed_size, 20, &cfg, notosansRange);*/

    ImGui_ImplPlume_CreateFontsTexture();

    camera = new Camera{ screenWidth, screenHeight };

    instance = this;

    return true;
}

static uint32_t imageIndex{ 0 };
static plume::RenderTexture* swapChainTexture;
void Graphics::renderBegin()
{
    renderCtx.commandList->begin();

    renderCtx.commandList->barriers(
        plume::RenderBarrierStage::GRAPHICS,
        plume::RenderTextureBarrier(renderCtx.screenTex.get(), plume::RenderTextureLayout::COLOR_WRITE)
    );
    renderCtx.commandList->setFramebuffer(renderCtx.screenFb.get());

    uint32_t width{ screenWidth };
    uint32_t height{ screenHeight };
    const plume::RenderViewport viewport{ 0.0f, 0.0f, (float)width, (float)height };
    const plume::RenderRect scissor{ 0, 0, (int32_t)width, (int32_t)height };

    renderCtx.commandList->setViewports(viewport);
    renderCtx.commandList->setScissors(scissor);

    plume::RenderColor clearColor{ 0.1f, 0.12f, 0.15f, 1.0f };
    renderCtx.commandList->clearColor(0, clearColor);
    renderCtx.commandList->clearDepth();
    renderCtx.commandList->clearDepthStencil();

    MainCBuffer* mainCbufferData = (MainCBuffer*)renderCtx.mainCBuffer->map();
    mainCbufferData->view = camera->viewMatrix();
    mainCbufferData->projection = camera->projMatrix();
    renderCtx.mainCBuffer->unmap();

    for (auto* model : models)
        model->render();
}

void Graphics::renderEnd() {
}

void Graphics::renderUIBegin() {
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ToolBar();

    ImGuiViewport* imguiViewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(imguiViewport->Pos);
    ImGui::SetNextWindowSize(imguiViewport->Size);
    ImGui::SetNextWindowViewport(imguiViewport->ID);

    ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
    ImGui::Begin("DockSpace Window", nullptr, windowFlags);
    ImGui::PopStyleVar(3);

    ImGui::SetCursorPosY(ImGui::GetFrameHeight());

    ImGui::DockSpace(ImGui::GetID("DockSpace"), { 0, 0 }, ImGuiDockNodeFlags_PassthruCentralNode);
}

void Graphics::renderUIEnd()
{
    ImGui::End();
    ImGui::Render();

    renderCtx.swapChain->acquireTexture(renderCtx.acquireSemaphore.get(), &imageIndex);

    swapChainTexture = renderCtx.swapChain->getTexture(imageIndex);
    renderCtx.commandList->barriers(plume::RenderBarrierStage::GRAPHICS, plume::RenderTextureBarrier(swapChainTexture, plume::RenderTextureLayout::COLOR_WRITE));

    const plume::RenderFramebuffer* framebuffer{ renderCtx.framebuffers[imageIndex].get() };
    renderCtx.commandList->setFramebuffer(framebuffer);

    uint32_t width{ renderCtx.swapChain->getWidth() };
    uint32_t height{ renderCtx.swapChain->getHeight() };
    const plume::RenderViewport viewport{ 0.0f, 0.0f, (float)width, (float)height };
    const plume::RenderRect scissor{ 0, 0, (int32_t)width, (int32_t)height };

    renderCtx.commandList->setViewports(viewport);
    renderCtx.commandList->setScissors(scissor);

    plume::RenderColor clearColor{ 0.1f, 0.12f, 0.15f, 1.0f };
    renderCtx.commandList->clearColor(0, clearColor);

    ImGui_ImplPlume_RenderDrawData(ImGui::GetDrawData(), renderCtx.commandList.get());

    renderCtx.commandList->barriers(plume::RenderBarrierStage::NONE, plume::RenderTextureBarrier{ swapChainTexture, plume::RenderTextureLayout::PRESENT });

    renderCtx.commandList->end();

    while (renderCtx.releaseSemaphores.size() < renderCtx.swapChain->getTextureCount())
        renderCtx.releaseSemaphores.emplace_back(renderCtx.device->createCommandSemaphore());

    const plume::RenderCommandList* cmdList = renderCtx.commandList.get();
    plume::RenderCommandSemaphore* waitSemaphore = renderCtx.acquireSemaphore.get();
    plume::RenderCommandSemaphore* signalSemaphore = renderCtx.releaseSemaphores[imageIndex].get();

    renderCtx.commandQueue->executeCommandLists(&cmdList, 1, &waitSemaphore, 1, &signalSemaphore, 1, renderCtx.fence.get());

    renderCtx.swapChain->present(imageIndex, &signalSemaphore, 1);
    renderCtx.commandQueue->waitForCommandFence(renderCtx.fence.get());
}

void Graphics::shutdown()
{
    if (auto* vkDevice = dynamic_cast<plume::VulkanDevice*>(renderCtx.device.get()))
        vkDeviceWaitIdle(vkDevice->vk);

    ImGui_ImplPlume_Shutdown();

    for (auto* model : models)
        model->shutdown();

    renderCtx.commandList.reset();
    renderCtx.fence.reset();
    renderCtx.commandFence.reset();
    renderCtx.acquireSemaphore.reset();
    renderCtx.releaseSemaphores.clear();

    renderCtx.framebuffers.clear();
    renderCtx.screenDs.reset();
    renderCtx.screenFb.reset();
    renderCtx.screenTex.reset();
    renderCtx.screenDTexView.reset();
    renderCtx.screenDTex.reset();
    renderCtx.mainCBuffer.reset();

    renderCtx.swapChain.reset();

    renderCtx.commandQueue.reset();

    renderCtx.device.reset();

    renderCtx.renderInterface.reset();
}

Graphics::~Graphics()
{
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    SDL_DestroyWindow(window);
    SDL_Quit();
}
