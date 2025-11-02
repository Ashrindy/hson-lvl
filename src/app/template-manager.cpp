#include "template-manager.h"
#include <hedgelib/sets/hl_set_obj_type.h>
#include <filesystem>

using namespace ulvl::app;
namespace fs = std::filesystem;

TemplateManager::Template::Template(const char* templateName) : name{ templateName } {
	fs::path hsonFile = fs::path{ "templates" } / templateName / "hson.json";
	hsonTemplate = new hl::set_object_type_database{ hsonFile.wstring().c_str() };
}

void TemplateManager::setTemplate(const char* templateName) {
	for (auto* temp : templates)
		if (strcmp(temp->name, templateName) == 0) {
			setTemplate(temp);
			return;
		}
}

void TemplateManager::setTemplate(Template* temp) {
	currentTemplate = temp;
}

void TemplateManager::loadTemplate(const char* templateName) {
	auto* temp = new Template{ templateName };
	templates.push_back(temp);
	if (currentTemplate == nullptr)
		setTemplate(temp);
}
