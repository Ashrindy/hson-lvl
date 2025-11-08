#include "template-manager.h"

using namespace ulvl::app;

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
