cmake_minimum_required (VERSION 3.8)
project (sendacopia)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# winmm.dll
add_library(winmm SHARED "src/dll/winmm.c")

# sendacopia.dll
add_library(${PROJECT_NAME} SHARED 
	"src/dll/sendacopia.c"
	"src/gui/gui.c"
	"src/gui/newfile.c"
	"src/sendacopia.c"
)

add_subdirectory(submodules/minhook)
target_include_directories(${PROJECT_NAME} PRIVATE submodules/minhook/include)

#set(MW_BUILD_STATIC ON)
#set(MW_TRY_VULKAN OFF)
add_subdirectory(submodules/milsko)
target_include_directories(${PROJECT_NAME} PRIVATE submodules/milsko/include)
message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/submodules/milsko)
target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/submodules/milsko)

target_link_libraries(${PROJECT_NAME} minhook Mw dbghelp)