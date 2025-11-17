#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <plume_render_interface.h>

namespace ulvl::gfx {
	struct BaseVertex {
		glm::vec3 position;
		glm::vec2 texcoord;
	};

	class BaseModel {
	protected:
		void* vertices{ nullptr };
		size_t vertexCount{ 0 };
		std::vector<plume::RenderInputElement> vertexLayout;
	public:
		std::vector<unsigned short> indices;
	protected:
		std::unique_ptr<plume::RenderPipeline> pipeline;
		std::unique_ptr<plume::RenderPipelineLayout> pipelineLayout;
		std::unique_ptr<plume::RenderBuffer> vertexBuffer;
		std::unique_ptr<plume::RenderBuffer> indexBuffer;
		std::unique_ptr<plume::RenderDescriptorSet> descriptor;
		std::vector<plume::RenderVertexBufferView> vertexBufferView;
		plume::RenderIndexBufferView indexBufferView;
		std::vector<plume::RenderInputSlot> inputSlots;
		unsigned int vertexStride{ sizeof(BaseVertex) };
		bool visible{ true };
	public:
		BaseModel();

		virtual void init();
		virtual void shutdown();

		inline void setVisibility(bool visible) {
			this->visible = visible;
		}

		size_t getVertexLayoutOffset(const char* semanticName) const;

		template<typename T>
		T& getVertexValue(size_t idx, size_t offset) {
			return *(T*)&((char*)vertices)[idx * vertexStride + offset];
		}

		virtual void render() = 0;
	};
}
