#pragma once
#include "service.h"
#include "resources/resource.h"

namespace ulvl::app {
	class ResourceManager : public Service {
	public:
		std::vector<Resource*> resources;

		bool hasResource(const char* name) const;
		inline bool hasResource(const std::string& name) const { return hasResource(name.c_str()); }

		Resource* getResource(const char* name);
		inline Resource* getResource(const std::string& name) { return getResource(name.c_str()); }

		inline Resource* createResource(const Resource::Class* resClass) { return resClass->createFunc(); }
		template<typename T>
		inline Resource* createResource() { return createResource(T::GetClass()); }

		Resource* createResource(const Resource::Class* resClass, const char* name);
		inline Resource* createResource(const Resource::Class* resClass, const std::string& name) { return createResource(resClass, name.c_str()); }
		template<typename T>
		inline Resource* createResource(const char* name) { return createResource(T::GetClass(), name); }
		template<typename T>
		inline Resource* createResource(const std::string& name) { return createResource<T>(name.c_str()); }

		Resource* createResource(const Resource::Class* resClass, const char* name, void* data, size_t size);
		inline Resource* createResource(const Resource::Class* resClass, const std::string& name, void* data, size_t size) { return createResource(resClass, name.c_str(), data, size); }
		template<typename T>
		inline Resource* createResource(const char* name, void* data, size_t size) { return createResource(T::GetClass(), name, data, size); }
		template<typename T, typename S>
		inline Resource* createResource(const char* name, S* data) { return createResource<T>(name, data, sizeof(S)); }
		template<typename T>
		inline Resource* createResource(const std::string& name, void* data, size_t size) { return createResource<T>(name.c_str(), data, size); }
		template<typename T, typename S>
		inline Resource* createResource(const std::string& name, S* data) { return createResource<T, S>(name.c_str(), data, sizeof(S)); }
	};
}
