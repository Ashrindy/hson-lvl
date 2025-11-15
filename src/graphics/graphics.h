#pragma once
#include "model.h"
#include "camera.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_properties.h>
#include <plume_vulkan.h>
#include <plume_render_interface.h>

/*
* Initialization and use of plume taken from plume's example.
*/

namespace ulvl::gfx {
	struct MainCBuffer {
		glm::mat4 view;
		glm::mat4 projection;
	};

	class Graphics {
	public:
		static Graphics* instance;

		struct RenderContext {
			std::unique_ptr<plume::RenderInterface> renderInterface;
			plume::RenderWindow renderWindow{};
			std::unique_ptr<plume::RenderDevice> device;
			std::unique_ptr<plume::RenderCommandQueue> commandQueue;
			std::unique_ptr<plume::RenderCommandList> commandList;
			std::unique_ptr<plume::RenderCommandFence> fence;
			std::unique_ptr<plume::RenderSwapChain> swapChain;
			std::unique_ptr<plume::RenderCommandSemaphore> acquireSemaphore;
			std::vector<std::unique_ptr<plume::RenderCommandSemaphore>> releaseSemaphores;
			std::unique_ptr<plume::RenderCommandFence> commandFence;
			std::vector<std::unique_ptr<plume::RenderFramebuffer>> framebuffers;
			std::unique_ptr<plume::RenderBuffer> mainCBuffer;

			std::unique_ptr<plume::RenderTexture> screenTex;
			std::unique_ptr<plume::RenderFramebuffer> screenFb;
			std::unique_ptr<plume::RenderDescriptorSet> screenDs;
		};

		void createFramebuffers();
		void createScreenFramebuffers();
		void resize(unsigned int width, unsigned int height);
		void resizeScreen(unsigned int width, unsigned int height);
		void reposScreen(unsigned int x, unsigned int y);
		void initializeRenderResources();
		void setUnsaved(bool unsaved);

		SDL_Window* window;
		unsigned int width{ 1280 }, height{ 720 };
		unsigned int screenWidth{ 1280 }, screenHeight{ 720 };
		unsigned int screenPosX{ 0 }, screenPosY{ 0 };
		float deltaTime{ 0 };
		Camera* camera;
		const char* name{ "hson-lvl" };
		std::vector<Model*> models;
		RenderContext renderCtx;

		bool init();
		void renderBegin();
		void renderEnd();
		void renderUIBegin();
		void renderUIEnd();
		void shutdown();
		~Graphics();
	};
}
