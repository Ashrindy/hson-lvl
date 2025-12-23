#pragma once
#include "service.h"
#include "../graphics/basemodel.h"

namespace ulvl::app {
	struct MeshData {
		struct VertSet {
			// TODO: Needs to be refcounted, as right now it's memory leaking
			void* vertices{ nullptr };
			gfx::VertexInfo vertexInfo{};

			template<typename T, size_t N>
			inline void setVertices(T(&verts)[N]) {
				if (vertices)
					delete vertices;

				vertices = new T[N]{};
				memcpy(vertices, verts, sizeof(T) * N);
			}
		};

		std::vector<VertSet> vertsets;
		unsigned short* indices{ nullptr };
		int vertexCount{ -1 };
		int indexCount{ -1 };

		~MeshData();
		template<size_t N>
		inline void setIndices(unsigned short (&inds)[N]) {
			if (indices)
				delete indices;

			indices = new unsigned short[N];
			memcpy(indices, &inds, sizeof(unsigned short) * N);
			indexCount = N;
		}
		VertSet& getVertices(const std::vector<plume::RenderInputElement>& vertLayout);
	};

	struct ModelData {
		std::vector<MeshData> meshes{};
		unsigned int nameHash{ 0 };
		bool inited{ false };

		inline bool uninited() { return !inited; }
	};

	class ModelService : public Service {
	public:
		std::vector<ModelData*> modelDatas;

		ModelData* getModel(unsigned int nameHash);
	};
}
