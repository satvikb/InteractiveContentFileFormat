cmake_minimum_required(VERSION 3.21)
set(OPENSSL_ROOT_DIR "C:/Users/Satvik/Documents/Development/vcpkg/installed/x86-windows")
# set the project name and version
project(InteractiveFileFormat VERSION 1.0)
file(GLOB SOURCES "*")

add_library(MAIN SHARED ${SOURCES})



find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})
add_executable(InteractiveFileFormat cApp.cpp)


# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


#target_link_libraries(InteractiveFileFormat PUBLIC Core UI)



target_include_directories(InteractiveFileFormat PUBLIC 
						   "${PROJECT_BINARY_DIR}"
						   "${PROJECT_SOURCE_DIR}/Core"
						   "${PROJECT_SOURCE_DIR}/UI"
						   "${PROJECT_SOURCE_DIR}/UI/Components"
						   "${PROJECT_SOURCE_DIR}/UI/Content"
							)
include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/whoshuu/cpr.git GIT_TAG c8d33915dbd88ad6c92b258869b03aba06587ff9) # the commit hash for 1.5.0
FetchContent_MakeAvailable(cpr)


# target_sources(InteractiveFileFormat PUBLIC "./Core")

find_package(OpenSSL REQUIRED)
target_link_libraries(InteractiveFileFormat PUBLIC ${wxWidgets_LIBRARIES} Core UI Main cpr::cpr OpenSSL::SSL)