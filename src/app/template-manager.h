#pragma once
#include "service.h"
#include "template/template.h"

namespace ulvl::app {
	class TemplateManager : public Service {
	public:
		Template* currentTemplate;
		std::vector<Template*> templates;

		void setTemplate(const char* templateName);
		void setTemplate(Template* temp);
		void loadTemplate(const char* templateName);
	};
}
