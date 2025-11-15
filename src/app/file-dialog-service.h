#pragma once
#include "service.h"
#include <filesystem>
#include <nfd.h>

namespace ulvl::app {
	class FileDialogService : public Service {
	public:
		virtual void AddCallback() override;
		bool openDialog(nfdopendialogu8args_t args, std::filesystem::path& outPath);
		void open();
	};
}
