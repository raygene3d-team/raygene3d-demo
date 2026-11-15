cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

set(NAME raygene3d)
project(${NAME} LANGUAGES C CXX)

# Default build type for single-config generators (Makefiles, Ninja)
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Preprocessor definitions (applied to all targets of this project)
add_compile_definitions(NOMINMAX)

# Compiler specific stuff
if(MSVC)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

set(THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)

set(xatlas_source
  ${THIRDPARTY_DIR}/xatlas/xatlas.cpp
  ${THIRDPARTY_DIR}/xatlas/xatlas.h
)

set(imgui_source
  ${THIRDPARTY_DIR}/imgui/imconfig.h
  ${THIRDPARTY_DIR}/imgui/imgui.cpp
  ${THIRDPARTY_DIR}/imgui/imgui.h
  ${THIRDPARTY_DIR}/imgui/imgui_demo.cpp
  ${THIRDPARTY_DIR}/imgui/imgui_draw.cpp
  ${THIRDPARTY_DIR}/imgui/imgui_internal.h
  ${THIRDPARTY_DIR}/imgui/imgui_widgets.cpp
  ${THIRDPARTY_DIR}/imgui/imstb_rectpack.h
  ${THIRDPARTY_DIR}/imgui/imstb_textedit.h
  ${THIRDPARTY_DIR}/imgui/imstb_truetype.h
)

set(meshoptimizer_source
  ${THIRDPARTY_DIR}/meshoptimizer/src/meshoptimizer.h
  ${THIRDPARTY_DIR}/meshoptimizer/src/allocator.cpp
  ${THIRDPARTY_DIR}/meshoptimizer/src/clusterizer.cpp
  ${THIRDPARTY_DIR}/meshoptimizer/src/indexcodec.cpp
  ${THIRDPARTY_DIR}/meshoptimizer/src/indexgenerator.cpp
  ${THIRDPARTY_DIR}/meshoptimizer/src/overdrawanalyzer.cpp
  ${THIRDPARTY_DIR}/meshoptimizer/src/overdrawoptimizer.cpp
  ${THIRDPARTY_DIR}/meshoptimizer/src/quantization.cpp
  ${THIRDPARTY_DIR}/meshoptimizer/src/simplifier.cpp
  ${THIRDPARTY_DIR}/meshoptimizer/src/spatialorder.cpp
  ${THIRDPARTY_DIR}/meshoptimizer/src/stripifier.cpp
  ${THIRDPARTY_DIR}/meshoptimizer/src/vcacheanalyzer.cpp
  ${THIRDPARTY_DIR}/meshoptimizer/src/vcacheoptimizer.cpp
  ${THIRDPARTY_DIR}/meshoptimizer/src/vertexcodec.cpp
  ${THIRDPARTY_DIR}/meshoptimizer/src/vertexfilter.cpp
  ${THIRDPARTY_DIR}/meshoptimizer/src/vfetchanalyzer.cpp
  ${THIRDPARTY_DIR}/meshoptimizer/src/vfetchoptimizer.cpp
)

set(mikktspace_source
  ${THIRDPARTY_DIR}/mikktspace/mikktspace.h
  ${THIRDPARTY_DIR}/mikktspace/mikktspace.c
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

set(io_source
  broker/io/mode.h
  broker/io/mode.cpp
  broker/io/scope.h
  broker/io/scope.cpp
  broker/io/mode/gltf_converter.h
  broker/io/mode/gltf_converter.cpp
  broker/io/mode/obj_converter.h
  broker/io/mode/obj_converter.cpp
)

set(SOURCE main.cpp
  broker/environment_broker.h
  broker/environment_broker.cpp
  broker/io_broker.h
  broker/io_broker.cpp
  broker/lightmap_broker.h
  broker/lightmap_broker.cpp
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
  ${io_source}
  ${lightmap_source}
  ${imgui_source}
  ${xatlas_source}
  ${mikktspace_source}
  ${meshoptimizer_source}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE})

# Apple: main.cpp is compiled as Objective-C++ (it uses Cocoa/Metal headers under __OBJC__).
# Applied per-file so that C/C++ third-party sources are not affected.
if(APPLE)
  set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/main.cpp PROPERTIES
    COMPILE_OPTIONS "-x;objective-c++"
  )
endif()

add_executable(${NAME}-demo ${SOURCE})

add_subdirectory(${NAME}-wrap)
add_subdirectory(${NAME}-util)
add_subdirectory(${NAME}-core)

target_link_libraries(${NAME}-demo PRIVATE
  ${NAME}-wrap
  ${NAME}-util
  ${NAME}-core
)

target_compile_features(${NAME}-demo PRIVATE cxx_std_20)

# SDL3 is vendored in 3rdparty/SDL and built with SDL's own CMake as a static
# library (no FetchContent). Subsystems the demo does not use are switched off
# with SDL's own SDL_<SUBSYSTEM> options.
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_INSTALL OFF CACHE BOOL "" FORCE)
set(SDL_AUDIO OFF CACHE BOOL "" FORCE)
set(SDL_CAMERA OFF CACHE BOOL "" FORCE)
set(SDL_DIALOG OFF CACHE BOOL "" FORCE)
set(SDL_GPU OFF CACHE BOOL "" FORCE)
set(SDL_HAPTIC OFF CACHE BOOL "" FORCE)
set(SDL_HIDAPI OFF CACHE BOOL "" FORCE)
set(SDL_JOYSTICK OFF CACHE BOOL "" FORCE)
set(SDL_NOTIFICATION OFF CACHE BOOL "" FORCE)
set(SDL_RENDER OFF CACHE BOOL "" FORCE)
set(SDL_SENSOR OFF CACHE BOOL "" FORCE)
set(SDL_TRAY OFF CACHE BOOL "" FORCE)
add_subdirectory(${THIRDPARTY_DIR}/SDL ${CMAKE_BINARY_DIR}/sdl3 EXCLUDE_FROM_ALL)

target_link_libraries(${NAME}-demo PRIVATE SDL3::SDL3-static)

if(WIN32)
#  target_link_libraries(${NAME}-demo PRIVATE
#    setupapi version imm32 winmm dxgi d3d11 gdi32 user32 ole32 oleaut32 shell32 advapi32 uuid
#  )
elseif(APPLE)
  target_link_libraries(${NAME}-demo PRIVATE
    "$<LINK_LIBRARY:FRAMEWORK,AppKit>"
    "$<LINK_LIBRARY:FRAMEWORK,IOKit>"
    "$<LINK_LIBRARY:FRAMEWORK,QuartzCore>"
    "$<LINK_LIBRARY:FRAMEWORK,Metal>"
  )
else()
  find_package(Threads REQUIRED)
  target_link_libraries(${NAME}-demo PRIVATE
    Threads::Threads
    ${CMAKE_DL_LIBS}
  )
endif()
