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

set(SOURCE main.cpp
  imgui/imgui.h
  imgui/imgui.cpp
  spark/spark.h
  spark/spark.cpp
  broker/import_broker.h
  broker/import_broker.cpp
  broker/lightmap_broker.h
  broker/lightmap_broker.cpp
  broker/environment_broker.h
  broker/environment_broker.cpp
  ${imgui_source}
  ${xatlas_source}
  ${mikktspace_source}
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE})

add_executable(${NAME}-demo
  ${SOURCE}
)

add_subdirectory(${NAME}-data)
#add_dependencies(${NAME}-demo ${NAME}-util)
target_link_libraries(${NAME}-demo PUBLIC ${NAME}-data)

add_subdirectory(${NAME}-core)
#add_dependencies(${NAME}-demo ${NAME}-core)
target_link_libraries(${NAME}-demo PUBLIC ${NAME}-core)

add_subdirectory(${NAME}-root)
#add_dependencies(${NAME}-demo ${NAME}-root)
target_link_libraries(${NAME}-demo PUBLIC ${NAME}-root)

#set(GLFW_LIB_PATH ${CMAKE_SOURCE_DIR}/3rdparty/glfw/lib)

#set(GLSLANG_LIB_PATH ${CMAKE_SOURCE_DIR}/3rdparty/glslang/lib)
#set(SPIRV_LIB_PATH ${CMAKE_SOURCE_DIR}/3rdparty/SPIRV/lib)
#set(SPIRVTOOLS_LIB_PATH ${CMAKE_SOURCE_DIR}/3rdparty/spirv-tools/lib)

if(WIN32)
	set(GLFW_LIB_PATH ${CMAKE_SOURCE_DIR}/3rdparty/glfw/lib/win64)
	set(GLSLANG_LIB_PATH ${CMAKE_SOURCE_DIR}/3rdparty/glslang/lib/win64)
	set(SPIRV_LIB_PATH ${CMAKE_SOURCE_DIR}/3rdparty/SPIRV/lib/win64)
	set(SPIRVTOOLS_LIB_PATH ${CMAKE_SOURCE_DIR}/3rdparty/spirv-tools/lib/win64)
endif(WIN32)

if(UNIX)
	if(APPLE)
		set(GLFW_LIB_PATH ${CMAKE_SOURCE_DIR}/3rdparty/glfw/lib/macos)
		set(GLSLANG_LIB_PATH ${CMAKE_SOURCE_DIR}/3rdparty/glslang/lib/macos)
		set(SPIRV_LIB_PATH ${CMAKE_SOURCE_DIR}/3rdparty/SPIRV/lib/macos)
		set(SPIRVTOOLS_LIB_PATH ${CMAKE_SOURCE_DIR}/3rdparty/spirv-tools/lib/macos)
	else(APPLE)
		set(GLFW_LIB_PATH ${CMAKE_SOURCE_DIR}/3rdparty/glfw/lib/linux)
		set(GLSLANG_LIB_PATH ${CMAKE_SOURCE_DIR}/3rdparty/glslang/lib/linux)
		set(SPIRV_LIB_PATH ${CMAKE_SOURCE_DIR}/3rdparty/SPIRV/lib/linux)
		set(SPIRVTOOLS_LIB_PATH ${CMAKE_SOURCE_DIR}/3rdparty/spirv-tools/lib/linux)
	endif(APPLE)
endif(UNIX)

if(WIN32)
target_link_libraries(${NAME}-demo PRIVATE
	debug ${GLFW_LIB_PATH}/glfw3d.lib optimized ${GLFW_LIB_PATH}/glfw3.lib
)
target_link_libraries(${NAME}-demo PRIVATE
	debug ${GLSLANG_LIB_PATH}/glslangd.lib optimized ${GLSLANG_LIB_PATH}/glslang.lib
	debug ${GLSLANG_LIB_PATH}/HLSLd.lib optimized ${GLSLANG_LIB_PATH}/HLSL.lib
	debug ${GLSLANG_LIB_PATH}/OGLCompilerd.lib optimized ${GLSLANG_LIB_PATH}/OGLCompiler.lib
	debug ${GLSLANG_LIB_PATH}/OSDependentd.lib optimized ${GLSLANG_LIB_PATH}/OSDependent.lib
	debug ${GLSLANG_LIB_PATH}/GenericCodeGend.lib optimized ${GLSLANG_LIB_PATH}/GenericCodeGen.lib
	debug ${GLSLANG_LIB_PATH}/MachineIndependentd.lib optimized ${GLSLANG_LIB_PATH}/MachineIndependent.lib
)
target_link_libraries(${NAME}-demo PRIVATE
	debug ${SPIRV_LIB_PATH}/SPIRVd.lib optimized ${SPIRV_LIB_PATH}/SPIRV.lib
	debug ${SPIRV_LIB_PATH}/SPVRemapperd.lib optimized ${SPIRV_LIB_PATH}/SPVRemapper.lib
	debug ${SPIRV_LIB_PATH}/SPIRVd.lib optimized ${SPIRV_LIB_PATH}/SPIRV.lib
)
target_link_libraries(${NAME}-demo PRIVATE
	debug ${SPIRVTOOLS_LIB_PATH}/SPIRV-Toolsd.lib optimized ${SPIRVTOOLS_LIB_PATH}/SPIRV-Tools.lib
	debug ${SPIRVTOOLS_LIB_PATH}/SPIRV-Tools-optd.lib optimized ${SPIRVTOOLS_LIB_PATH}/SPIRV-Tools-opt.lib
	debug ${SPIRVTOOLS_LIB_PATH}/SPIRV-Tools-reduced.lib optimized ${SPIRVTOOLS_LIB_PATH}/SPIRV-Tools-reduce.lib
)
endif(WIN32)

if(UNIX)
	if(APPLE)
		target_link_libraries(${NAME}-demo PRIVATE
			optimized ${GLFW_LIB_PATH}/libglfw3.a
		)
		target_link_libraries(${NAME}-demo PRIVATE
			optimized ${GLSLANG_LIB_PATH}/libMachineIndependent.a
			optimized ${GLSLANG_LIB_PATH}/libGenericCodeGen.a
			optimized ${GLSLANG_LIB_PATH}/libglslang.a
			optimized ${GLSLANG_LIB_PATH}/libHLSL.a
			optimized ${GLSLANG_LIB_PATH}/libOGLCompiler.a
			optimized ${GLSLANG_LIB_PATH}/libOSDependent.a
		)
		target_link_libraries(${NAME}-demo PRIVATE
			optimized ${SPIRV_LIB_PATH}/libSPIRV.a
			optimized ${SPIRV_LIB_PATH}/libSPVRemapper.a
			optimized ${SPIRV_LIB_PATH}/libSPIRV.a
		)
		target_link_libraries(${NAME}-demo PRIVATE
			optimized ${SPIRVTOOLS_LIB_PATH}/libSPIRV-Tools.a
			optimized ${SPIRVTOOLS_LIB_PATH}/libSPIRV-Tools-opt.a
			optimized ${SPIRVTOOLS_LIB_PATH}/libSPIRV-Tools-reduce.a
		)
		target_link_libraries(${NAME}-demo PRIVATE
			"-framework AppKit"
			"-framework IOKit"
			"-framework QuartzCore"
			"-framework Metal"
		)
	else(APPLE)
		target_link_libraries(${NAME}-demo PRIVATE
			optimized ${GLFW_LIB_PATH}/libglfw3.a
		)
		target_link_libraries(${NAME}-demo PRIVATE
			optimized ${GLSLANG_LIB_PATH}/libMachineIndependent.a
			optimized ${GLSLANG_LIB_PATH}/libGenericCodeGen.a
			optimized ${GLSLANG_LIB_PATH}/libglslang.a
			optimized ${GLSLANG_LIB_PATH}/libHLSL.a
			optimized ${GLSLANG_LIB_PATH}/libOGLCompiler.a
			optimized ${GLSLANG_LIB_PATH}/libOSDependent.a
		)
		target_link_libraries(${NAME}-demo PRIVATE
			optimized ${SPIRV_LIB_PATH}/libSPIRV.a
			optimized ${SPIRV_LIB_PATH}/libSPVRemapper.a
			optimized ${SPIRV_LIB_PATH}/libSPIRV.a
		)
		target_link_libraries(${NAME}-demo PRIVATE
			optimized ${SPIRVTOOLS_LIB_PATH}/libSPIRV-Tools.a
			optimized ${SPIRVTOOLS_LIB_PATH}/libSPIRV-Tools-opt.a
			optimized ${SPIRVTOOLS_LIB_PATH}/libSPIRV-Tools-reduce.a
		)
		target_link_libraries(${NAME}-demo PRIVATE
			optimized -ldl
			optimized -lpthread
		)
	endif(APPLE)
endif(UNIX)
