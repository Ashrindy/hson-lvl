#pragma once
#include "service.h"
#include "applistener.h"
#include <filesystem>
#include <nfd.h>

namespace ulvl::app {
	class FileDialogService : public Service, public ApplicationListener {
	public:
		struct DialogDesc : nfdopendialogu8args_t {
			bool folderDialog{ false };
		};

		virtual void AddCallback() override;
		bool openDialog(DialogDesc& dialogDesc, std::filesystem::path& outPath);
		void openLayer();
		void openProject();
		bool canOpen() const;
		virtual void EventCallback(SDL_Event e) override;
	};
}
