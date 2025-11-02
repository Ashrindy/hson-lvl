#pragma once
#include "service.h"
#include <hedgelib/hh/hl_hh_gedit.h>

namespace ulvl::app {
	class TemplateManager : public Service {
	public:
		struct Template {
			const char* name;
			hl::set_object_type_database* hsonTemplate;

			Template(const char* templateName);
		};

		Template* currentTemplate;
		std::vector<Template*> templates;

		void setTemplate(const char* templateName);
		void setTemplate(Template* temp);
		void loadTemplate(const char* templateName);
	};
}
