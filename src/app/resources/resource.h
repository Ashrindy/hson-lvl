#pragma once

#define RESOURCE(TYPE) public: \
	static Resource* Create() { return new TYPE{}; } \
	static inline const Class staticClass{ NAME_HASH(#TYPE), &TYPE::Create }; \
	static inline const Class* GetClass() { return &staticClass; }

namespace ulvl::app {
	class Resource {
	public:
		typedef Resource* CreateFunction(void);

		struct Class {
			unsigned int name{ 0 };
			CreateFunction* createFunc{ nullptr };

			Class() {}
			Class(unsigned int name, CreateFunction* createFunc) : name{ name }, createFunc{ createFunc } {}
		};

		std::string name{};
		void* binaryData{ nullptr };
		size_t size{ 0 };
		const Class* resourceClass{ nullptr };
		std::optional<std::filesystem::path> filepath{};

		virtual ~Resource() = default;
		virtual void Load(void* data, size_t size) = 0;
		virtual void Unload() = 0;
	};
}
