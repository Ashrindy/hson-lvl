#include "resource-manager.h"

using namespace ulvl::app;

bool ResourceManager::hasResource(const char* name) const {
	for (auto* res : resources)
		if (res->name == name)
			return true;
	return false;
}

Resource* ResourceManager::getResource(const char* name) {
	for (auto* res : resources)
		if (res->name == name)
			return res;
	return nullptr;
}

Resource* ResourceManager::createResource(const Resource::Class* resClass, const char* name) {
	if (auto* res = getResource(name))
		return res;

	return createResource(resClass);
}

Resource* ResourceManager::createResource(const Resource::Class* resClass, const char* name, void* data, size_t size) {
	Resource* res{ getResource(name) };
	if (res)
		return res;

	res = createResource(resClass);
	res->Load(data, size);
	return res;
}
