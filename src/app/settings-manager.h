#pragma once
#include "service.h"
#include <filesystem>

namespace ulvl::app {
	struct Settings {
		std::string selectedTemplateName{ "none" };

		void save(std::vector<char>& buffer);
		void load(std::vector<char>& buffer);
	};

	class SettingsManager : public Service {
	public:
		static constexpr const char* defaultFilename = "settings.json";
		Settings settings{};

		virtual void AddCallback() override;
		void save(std::filesystem::path& path);
		void load(std::filesystem::path& path);
		void save();
		void load();
	};
}
