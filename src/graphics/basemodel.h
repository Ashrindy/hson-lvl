#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <plume_render_interface.h>

// TODO: Refactor entire model system to be more universal

namespace ulvl::gfx {
	struct BaseVertex {
		glm::vec3 position;
	};

	struct ColorVertex {
		glm::vec3 position;
		glm::vec4 color;
	};

	struct VertexInfo {
		std::vector<plume::RenderInputElement> vertexLayout{};
		size_t stride{ 0 };

		VertexInfo() = default;
		VertexInfo(std::vector<plume::RenderInputElement>& vertexLayout) : vertexLayout{ vertexLayout } { calcStrideByLayout(); }
		void calcStrideByLayout();
	};

	struct VertexBuffer {
		struct Desc {
			plume::RenderInputSlotClassification slotClass{ plume::RenderInputSlotClassification::PER_VERTEX_DATA };
			std::vector<plume::RenderInputElement> vertexLayout{};
		};

		void* vertices{ nullptr };
		size_t vertexCount{ 0 };
		VertexInfo vertexInfo{};
		std::unique_ptr<plume::RenderBuffer> buffer{};
		plume::RenderVertexBufferView bufferView{};
		plume::RenderInputSlot inputSlot{};
		plume::RenderInputSlotClassification slotClass{ plume::RenderInputSlotClassification::PER_VERTEX_DATA };

		VertexBuffer() = default;
		inline VertexBuffer(std::vector<plume::RenderInputElement>& vertexLayout) : vertexInfo{ vertexLayout } {}
		inline VertexBuffer(std::vector<plume::RenderInputElement>& vertexLayout, plume::RenderInputSlotClassification slotClass) : vertexInfo{ vertexLayout }, slotClass{ slotClass } {}
		//inline ~VertexBuffer() { delete[] vertices; }
		void setVertices(void* newVertices, unsigned int count);
		void addVertices(void* newVertices, unsigned int count);
	};

	struct IndexBuffer {
		std::vector<unsigned short> indices{};
		std::unique_ptr<plume::RenderBuffer> buffer{};
		plume::RenderIndexBufferView bufferView{};

		IndexBuffer() = default;
		void setIndices(unsigned short* newIndices, unsigned int count);
		void addIndices(unsigned short* newIndices, unsigned int count);
	};

	struct Shader {
		const unsigned char* data{ nullptr };
		size_t size{ 0 };

		Shader() = default;
		Shader(const unsigned char* data, size_t size) : data{ data }, size{ size } {}
		std::unique_ptr<plume::RenderShader> getShader();
	};

	struct Buffer {
		void* data{ nullptr };
		size_t size{ 0 };

		Buffer() = default;
		Buffer(size_t size) : size{ size } {}

		void setData(void* data) {
			this->data = data;
		}
		template<typename T>
		void setData(T* data) {
			this->data = data;
			size = sizeof(T);
		}
	};

	struct RenderBuffer {
		plume::RenderBuffer* buffer{ nullptr };
		size_t size{ 0 };

		RenderBuffer() = default;
		RenderBuffer(plume::RenderBuffer* buffer, size_t size) : buffer{ buffer }, size{ size } {}
	};

	struct DescriptorSet {
		std::unique_ptr<plume::RenderDescriptorSet> descriptor{};
		std::vector<RenderBuffer> buffers{};

		DescriptorSet() = default;
		DescriptorSet(plume::RenderDescriptorSetDesc& desc);
	};

	struct Pipeline {
		struct Desc {
			struct Depth {
				bool enabled{ true };
				bool writeEnabled{ true };
				plume::RenderComparisonFunction function{ plume::RenderComparisonFunction::LESS };
			};

			const unsigned char* vertexShader{ nullptr };
			size_t vertexShaderSize{ 0 };
			const unsigned char* pixelShader{ nullptr };
			size_t pixelShaderSize{ 0 };
			plume::RenderPrimitiveTopology primitiveTopo{ plume::RenderPrimitiveTopology::TRIANGLE_LIST };
			plume::RenderCullMode cullMode{ plume::RenderCullMode::NONE };
			Depth depth{};
			std::vector<plume::RenderDescriptorSetDesc> descriptorSetDescs{};
			std::vector<plume::RenderPushConstantRange> pushConstantDescs{};
			std::vector<VertexBuffer::Desc> vertexBufferDescs{};
		};

		std::unique_ptr<plume::RenderPipeline> pipeline{};
		std::unique_ptr<plume::RenderPipelineLayout> pipelineLayout{};
		std::vector<DescriptorSet> descriptors{};
		std::vector<Buffer> pushConstants{};
		std::vector<VertexBuffer> vertexBuffers{};
		IndexBuffer indexBuffer{};
		Shader vertexShader{};
		Shader pixelShader{};
		Desc description{};

	private:
		std::vector<plume::RenderVertexBufferView> vertexBufferViews{};
		std::vector<plume::RenderInputSlot> inputSlots{};

	public:
		Pipeline() = default;
		void init(Desc& desc);
		void setVertices(void* vertices, unsigned int count, unsigned int vertexBufferIndex);
		void addVertices(void* vertices, unsigned int count, unsigned int vertexBufferIndex);
		inline void setIndices(unsigned short* indices, unsigned int count) { indexBuffer.setIndices(indices, count); }
		inline void addIndices(unsigned short* indices, unsigned int count) { indexBuffer.addIndices(indices, count); }
		inline void setIndices(std::vector<unsigned short>& indices) { setIndices(indices.data(), indices.size()); }
		inline void addIndices(std::vector<unsigned short>& indices) { addIndices(indices.data(), indices.size()); }
		void render();
		void shutdown();

	private:
		void updateVertexBufferViews();
	};

	class BaseModel {
	protected:
		bool visible{ true };
	public:
		Pipeline pipeline{};

		struct ModelDesc {
			plume::RenderPrimitiveTopology primitiveTopo{ plume::RenderPrimitiveTopology::TRIANGLE_LIST };
			plume::RenderCullMode cullMode{ plume::RenderCullMode::NONE };
			const unsigned char* vertexShader{ nullptr };
			size_t vertexShaderSize{ 0 };
			const unsigned char* pixelShader{ nullptr };
			size_t pixelShaderSize{ 0 };
			std::vector<plume::RenderInputElement> vertexLayout{
				{ "POSITION", 0, 0, plume::RenderFormat::R32G32B32_FLOAT, 0, 0 }
			};
		};

		BaseModel() = default;
		BaseModel(ModelDesc desc);

		virtual void init(ModelDesc desc);
		virtual void shutdown();

		inline void setVisibility(bool visible) {
			this->visible = visible;
		}

		virtual void render() = 0;
	};
}
