include(FetchContent)

FetchContent_Declare(glm		GIT_REPOSITORY https://github.com/g-truc/glm.git)
FetchContent_Declare(gli  GIT_REPOSITORY https://github.com/g-truc/gli.git)
FetchContent_Declare(squirrel	GIT_REPOSITORY https://github.com/albertodemichelis/squirrel.git)
FetchContent_Declare(yyjson		GIT_REPOSITORY https://github.com/ibireme/yyjson.git)
set(DISABLE_DYNAMIC ON CACHE BOOL "" FORCE)
set(DISABLE_STATIC OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glm gli squirrel yyjson)

find_package(Vulkan REQUIRED)

add_subdirectory("${HE_LEVEL_EXTERNAL_DIR}/imgui")
add_subdirectory("${HE_LEVEL_EXTERNAL_DIR}/imguizmo")
add_subdirectory("${HE_LEVEL_EXTERNAL_DIR}/hedgelib")
set(SDL_VULKAN_ENABLED ON CACHE BOOL "")
add_subdirectory("${HE_LEVEL_EXTERNAL_DIR}/plume")
add_subdirectory("${HE_LEVEL_EXTERNAL_DIR}/SDL")
add_subdirectory("${HE_LEVEL_EXTERNAL_DIR}/imgui-impl-plume")
add_subdirectory("${HE_LEVEL_EXTERNAL_DIR}/nativefiledialog-extended")

target_link_libraries(${HE_LEVEL_NAME} PRIVATE imgui imguizmo glm gli plume Vulkan::Vulkan SDL3::SDL3 HedgeLib imgui-impl-plume squirrel_static sqstdlib_static nfd yyjson)
