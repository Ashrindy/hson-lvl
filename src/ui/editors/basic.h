#pragma once

namespace ulvl {
	bool Editor(const char* label, bool& value);
	bool Editor(const char* label, unsigned int& value);
	bool Editor(const char* label, int& value);
	bool Editor(const char* label, unsigned long long& value);
	bool Editor(const char* label, long long& value);
	bool Editor(const char* label, float& value);
	bool Editor(const char* label, double& value);
	bool Editor(const char* label, std::string& value);
	void Editor(const char* label, const char* value);
}
