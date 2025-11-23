#include "file-dialog-service.h"
#include "project-manager.h"
#include "template-manager.h"
#include "../app.h"

using namespace ulvl::app;

void FileDialogService::AddCallback() {
	NFD_Init();
	Application::instance->addListener(this);
}

bool FileDialogService::openDialog(DialogDesc& dialogDesc, std::filesystem::path& outPath) {
	nfdu8char_t* out{ nullptr };
	nfdresult_t result{ NFD_ERROR };
	if (dialogDesc.folderDialog) {
		nfdpickfolderu8args_t pickFolderDesc{
			.defaultPath = dialogDesc.defaultPath
		};
		result = NFD_PickFolderU8_With(&out, &pickFolderDesc);
	}
	else result = NFD_OpenDialogU8_With(&out, &dialogDesc);
	if (result == NFD_OKAY) {
		outPath = std::filesystem::path{ out };
		NFD_FreePathU8(out);
		return 1;
	}

	return 0;
}

void FileDialogService::openLayer() {
	if (!canOpen()) return;

	std::filesystem::path out{};
	nfdu8filteritem_t filters = { "Layer", "hson" };
	DialogDesc args{};
	args.filterCount = 1;
	args.filterList = &filters;
	if (openDialog(args, out)) {
		Application::instance->getService<ProjectManager>()->loadLayer(out);
	}
}

void FileDialogService::openProject() {
	if (!canOpen()) return;

	std::filesystem::path out{};
	DialogDesc args{};
	args.folderDialog = true;
	if (openDialog(args, out)) {
		Application::instance->getService<ProjectManager>()->loadProject(out);
	}
}

bool FileDialogService::canOpen() const {
	return Application::instance->getService<TemplateManager>()->currentTemplate;
}

void FileDialogService::EventCallback(SDL_Event e) {
	if (!canOpen()) return;

	const bool* keys = SDL_GetKeyboardState(NULL);

	if ((keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL]) && (keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT]) && keys[SDL_SCANCODE_O]) openProject();
	else if ((keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL]) && keys[SDL_SCANCODE_O]) openLayer();
}
