cmake_minimum_required (VERSION 3.8)
project (sendacopia)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# winmm.dll
add_library(winmm SHARED "src/winmm.c")

# sendacopia.dll
add_library(${PROJECT_NAME} SHARED "src/sendacopia.c")

add_subdirectory(minhook)
target_include_directories(${PROJECT_NAME} PRIVATE minhook/include)
target_link_libraries(${PROJECT_NAME} minhook)