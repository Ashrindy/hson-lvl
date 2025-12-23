add_executable (${HE_LEVEL_NAME}
	"he-level.cpp" 
	"app.cpp" 
	"app.h" 
	"log.h" 
	"log.cpp" 
	"types.h"

	"graphics/graphics.cpp" 
	"graphics/graphics.h"
	"graphics/camera.h" 
	"graphics/camera.cpp"
	"graphics/basemodel.h"
	"graphics/basemodel.cpp"
	"graphics/model.h" 
	"graphics/model.cpp"
	"graphics/instancedmodel.h"
	"graphics/instancedmodel.cpp"
	"graphics/fonts/Inter.h"
#	"graphics/fonts/NotoSansJP.h"
	"graphics/shaders/vs.h"
	"graphics/shaders/ps.h"
	"graphics/shaders/instance_vs.h"
	"graphics/shaders/color_ps.h"
	"graphics/shaders/color_vs.h"

	"math/aabb.h"
	"math/ray.h"
	"math/ray.cpp"
	"math/mat.h"

	"app/service.h" 
	"app/applistener.h"
	"app/camera-service.h"
	"app/camera-service.cpp"
 	"app/object-service.h"
 	"app/object-service.cpp"
	"app/project-manager.h" 
	"app/project-manager.cpp"
	"app/object-selection-manager.h"
	"app/object-selection-manager.cpp"
	"app/template-manager.h"
	"app/template-manager.cpp"
	"app/debug-visual-service.h"
	"app/debug-visual-service.cpp"
	"app/file-dialog-service.h"
	"app/file-dialog-service.cpp"
	"app/cleaner-service.h"
	"app/cleaner-service.cpp"
	"app/settings-manager.h"
	"app/settings-manager.cpp"
	"app/model-service.h"
	"app/model-service.cpp"

	"app/template/template.h"
	"app/template/template.cpp"
	"app/template/squirrelwrap.h"
	"app/template/squirrelwrap.cpp"
	"app/template/sq-funcs.h"
	"app/template/sq-funcs.cpp"
	"app/template/sq-types.h" 
	
    "panels/object-inspector.h"
	"panels/object-inspector.cpp" 
	"panels/object-library.h"
	"panels/object-library.cpp"
	"panels/viewport.h" 
	"panels/viewport.cpp"
	"panels/object-list.h"
	"panels/object-list.cpp"
	"panels/layer-properties.h"
	"panels/layer-properties.cpp"
	"panels/settings.h"
	"panels/settings.cpp"

	"ui/toolbar.h"
	"ui/panel.h"
	"ui/panel.cpp"

	"ui/editors/glm.h" 
	"ui/editors/glm.cpp" 
	"ui/editors/basic.h"
	"ui/editors/basic.cpp"

	"utilities/hson.h" 
	"utilities/hson.cpp"
	"utilities/namehash.h"
)

target_precompile_headers(${HE_LEVEL_NAME} PRIVATE "pch.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${HE_LEVEL_NAME} PROPERTY CXX_STANDARD 20)
endif()

add_custom_command(
    TARGET ${HE_LEVEL_NAME}
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${HE_LEVEL_TEMPLATES_DIR}
        ${CMAKE_BINARY_DIR}/src/templates
)
