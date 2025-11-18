#include "settings-manager.h"
#include <yyjson.h>
#include <fstream>

using namespace ulvl::app;

#define WRITESTRING(NAME) yyjson_mut_obj_add_str(doc, root, #NAME, NAME.c_str())
#define LOADSTRING(NAME) yyjson_val* NAME##Json = yyjson_obj_get(root, #NAME); NAME = yyjson_get_str(NAME##Json)

void Settings::save(std::vector<char>& buffer) {
    yyjson_mut_doc* doc{ yyjson_mut_doc_new(NULL) };
    yyjson_mut_val* root{ yyjson_mut_obj(doc) };
    yyjson_mut_doc_set_root(doc, root);

    WRITESTRING(selectedTemplateName);

    const char* json = yyjson_mut_write(doc, 0, NULL);
    if (json) {
        buffer.resize(strlen(json));
        strcpy(buffer.data(), json);
        free((void*)json);
    }

    yyjson_mut_doc_free(doc);
}

void Settings::load(std::vector<char>& buffer) {
    yyjson_doc* doc = yyjson_read(buffer.data(), buffer.size(), 0);
    yyjson_val* root = yyjson_doc_get_root(doc);

    LOADSTRING(selectedTemplateName);

    yyjson_doc_free(doc);
}

void SettingsManager::AddCallback() {
    load();
}

void SettingsManager::save(std::filesystem::path& path) {
    std::vector<char> data{};
    settings.save(data);
    std::ofstream file{ path, std::ios::binary };
    file.write(data.data(), data.size());
    file.close();
}

void SettingsManager::load(std::filesystem::path& path) {
    std::ifstream file{ path, std::ios::binary | std::ios::ate };
    if (!file) { save(path); return; }

    const std::streamsize size{ file.tellg() };
    file.seekg(0, std::ios::beg);

    std::vector<char> data(size);
    if (!file.read(data.data(), size))
        data.clear();
    file.close();

    settings.load(data);
}

void SettingsManager::save() {
    auto path = std::filesystem::path{ defaultFilename };
    save(path);
}

void SettingsManager::load() {
    auto path = std::filesystem::path{ defaultFilename };
    load(path);
}
