cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
cmake_policy(VERSION 3.7)

set(NAME raygene3d)
project(${NAME})


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set preprocessor defines
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-DNOMINMAX")

# Compiler specific stuff
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

if(APPLE)
	add_definitions("-x objective-c++")
endif(APPLE)

set(3RDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
set(signal_source
  ${3RDPARTY_DIR}/signals/signals.cpp
  ${3RDPARTY_DIR}/signals/signals.h
)

set(xatlas_source
  ${3RDPARTY_DIR}/xatlas/xatlas.cpp
  ${3RDPARTY_DIR}/xatlas/xatlas.h
)

set(imgui_source
  ${3RDPARTY_DIR}/imgui/imconfig.h
  ${3RDPARTY_DIR}/imgui/imgui.cpp
  ${3RDPARTY_DIR}/imgui/imgui.h
  ${3RDPARTY_DIR}/imgui/imgui_demo.cpp
  ${3RDPARTY_DIR}/imgui/imgui_draw.cpp
  ${3RDPARTY_DIR}/imgui/imgui_internal.h
  ${3RDPARTY_DIR}/imgui/imgui_widgets.cpp
  ${3RDPARTY_DIR}/imgui/imstb_rectpack.h
  ${3RDPARTY_DIR}/imgui/imstb_textedit.h
  ${3RDPARTY_DIR}/imgui/imstb_truetype.h
)

set(mikktspace_source
  ${3RDPARTY_DIR}/mikktspace/mikktspace.h
  ${3RDPARTY_DIR}/mikktspace/mikktspace.c
)

set(render_3d_source
  broker/render_3d/mode.h
  broker/render_3d/mode.cpp
  broker/render_3d/scope.h
  broker/render_3d/scope.cpp
  broker/render_3d/mode/no_shadow.h
  broker/render_3d/mode/no_shadow.cpp
  broker/render_3d/mode/cubemap_shadow.h
  broker/render_3d/mode/cubemap_shadow.cpp
  broker/render_3d/mode/sw_traced_shadow.h
  broker/render_3d/mode/sw_traced_shadow.cpp
  broker/render_3d/mode/hw_traced_shadow.h
  broker/render_3d/mode/hw_traced_shadow.cpp
)

set(lightmap_source
  broker/lightmap/mode.h
  broker/lightmap/mode.cpp
  broker/lightmap/scope.h
  broker/lightmap/scope.cpp
  broker/lightmap/mode/sw_traced_atlas.h
  broker/lightmap/mode/sw_traced_atlas.cpp
  broker/lightmap/mode/hw_traced_atlas.h
  broker/lightmap/mode/hw_traced_atlas.cpp
)


set(SOURCE main.cpp
  broker/environment_broker.h
  broker/environment_broker.cpp
  broker/import_broker.h
  broker/import_broker.cpp
  broker/lightmap_broker.h
  broker/lightmap_broker.cpp
  broker/mikktspace_broker.h
  broker/mikktspace_broker.cpp
  broker/photonmap_broker.h
  broker/photonmap_broker.cpp
  broker/render_3d_broker.h
  broker/render_3d_broker.cpp
  broker/render_ui_broker.h
  broker/render_ui_broker.cpp
  broker/scene_broker.h
  broker/scene_broker.cpp
  broker/trace_broker.h
  broker/trace_broker.cpp
  broker/xatlas_broker.h
  broker/xatlas_broker.cpp
  ${render_3d_source}
  ${lightmap_source}
  ${imgui_source}
  ${xatlas_source}
  ${mikktspace_source}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE})

add_executable(${NAME}-demo
  ${SOURCE}
)

add_subdirectory(${NAME}-wrap)
#add_dependencies(${NAME}-demo ${NAME}-wrap)
target_link_libraries(${NAME}-demo PUBLIC ${NAME}-wrap)

add_subdirectory(${NAME}-util)
#add_dependencies(${NAME}-demo ${NAME}-util)
target_link_libraries(${NAME}-demo PUBLIC ${NAME}-util)

add_subdirectory(${NAME}-core)
#add_dependencies(${NAME}-demo ${NAME}-core)
target_link_libraries(${NAME}-demo PUBLIC ${NAME}-core)

set(GLFW_LIB_PATH ${CMAKE_SOURCE_DIR}/3rdparty/glfw/lib)


if(WIN32)
	target_link_libraries(${NAME}-demo PRIVATE
		debug ${GLFW_LIB_PATH}/win64/glfw3d.lib optimized ${GLFW_LIB_PATH}/win64/glfw3.lib
)
endif(WIN32)

if(UNIX)
	if(APPLE)
		target_link_libraries(${NAME}-demo PRIVATE
			optimized ${GLFW_LIB_PATH}/macos/libglfw3.a
		)
		target_link_libraries(${NAME}-demo PRIVATE
			"-framework AppKit"
			"-framework IOKit"
			"-framework QuartzCore"
			"-framework Metal"
		)
	else(APPLE)
		target_link_libraries(${NAME}-demo PRIVATE
			optimized ${GLFW_LIB_PATH}/linux/libglfw3.a
		)
		target_link_libraries(${NAME}-demo PRIVATE
			optimized -ldl
			optimized -lpthread
		)
	endif(APPLE)
endif(UNIX)
