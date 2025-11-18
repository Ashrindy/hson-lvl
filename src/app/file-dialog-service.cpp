#include "file-dialog-service.h"
#include "project-manager.h"
#include "template-manager.h"
#include "../app.h"

using namespace ulvl::app;

void FileDialogService::AddCallback() {
	NFD_Init();
}

bool FileDialogService::openDialog(nfdopendialogu8args_t args, std::filesystem::path& outPath) {
	nfdu8char_t* out{ nullptr };
	nfdresult_t result{ NFD_OpenDialogU8_With(&out, &args) };
	if (result == NFD_OKAY) {
		outPath = std::filesystem::path{ out };
		NFD_FreePathU8(out);
		return 1;
	}

	return 0;
}

void FileDialogService::open() {
	if (!canOpen()) return;

	std::filesystem::path out{};
	nfdu8filteritem_t filters = { "Layer", "hson" };
	nfdopendialogu8args_t args{};
	args.filterCount = 1;
	args.filterList = &filters;
	if (openDialog(args, out)) {
		Application::instance->getService<ProjectManager>()->loadLayer(out);
	}
}

bool ulvl::app::FileDialogService::canOpen() const {
	return Application::instance->getService<TemplateManager>()->currentTemplate;
}
