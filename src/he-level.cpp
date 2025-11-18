#include "app.h"

int main(int argc, char** argv) {
	ulvl::Application* app = new ulvl::Application;
	app->init(argc, argv);
	app->loop();
	delete app;
	return 0;
}
