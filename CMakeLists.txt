cmake_minimum_required(VERSION 3.18)

project(MaBoSSG VERSION 0.1 LANGUAGES CUDA CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

include(FindCUDA/select_compute_arch)
CUDA_DETECT_INSTALLED_GPUS(INSTALLED_GPU_CCS_1)
string(STRIP "${INSTALLED_GPU_CCS_1}" INSTALLED_GPU_CCS_2)
string(REPLACE " " ";" INSTALLED_GPU_CCS_3 "${INSTALLED_GPU_CCS_2}")
string(REPLACE "." "" CUDA_ARCH_LIST "${INSTALLED_GPU_CCS_3}")
SET(CMAKE_CUDA_ARCHITECTURES ${CUDA_ARCH_LIST})

# Target MaBoSSGCore
file(GLOB_RECURSE src_files "src/*")
list(FILTER src_files EXCLUDE REGEX ".*main\\..*")

add_library(MaBoSSGCore ${src_files})

target_include_directories(MaBoSSGCore PUBLIC ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})

# Target unit_MaBoSSG
file(GLOB_RECURSE test_files "test/*")
add_executable(unit_MaBoSSG ${test_files})

# Target MaBoSSG
add_executable(MaBoSSG src/main)
target_link_libraries(MaBoSSG MaBoSSGCore nlohmann_json::nlohmann_json)

target_link_libraries(unit_MaBoSSG PRIVATE
	gtest_main gmock_main MaBoSSGCore)

target_include_directories(unit_MaBoSSG PUBLIC
	"src")

foreach(target MaBoSSG unit_MaBoSSG MaBoSSGCore)
	if(MSVC)
		target_compile_options(${target} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:/W4 /bigobj>)
	else()
		target_compile_options(${target} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wall -Wextra -pedantic>)
	endif()
    target_compile_options(${target} PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:--extended-lambda --expt-relaxed-constexpr>)
#   target_compile_options(${target} PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:--source-in-ptx -lineinfo>)
endforeach()

add_custom_command(TARGET unit_MaBoSSG POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:${PROJECT_NAME}>/data)

enable_testing()
include(GoogleTest)
gtest_discover_tests(unit_MaBoSSG)
