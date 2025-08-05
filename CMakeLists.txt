cmake_minimum_required(VERSION 3.19)
project(NDKVideoIO CXX C)

if(NOT CMAKE_SYSTEM_NAME)
    set(CMAKE_SYSTEM_NAME "Android") # Linux Android
endif()
if(NOT ANDROID_ABI)
    set(ANDROID_ABI "arm64-v8a") # Linux Android
endif()
set(CMAKE_ANDROID_STL_TYPE gnustl_static)
set(SYSTEM_ABI ${ANDROID_ABI})
set(BUILD_SHARED_LIBS ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_SKIP_RPATH ON) # For cross compiler CMAKE_SKIP_BUILD_RPATH=ON, Else CMAKE_SKIP_BUILD_RPATH=OFF

message(STATUS "Operate System : " ${CMAKE_SYSTEM_NAME})
message(STATUS "Compiler ID : " ${CMAKE_CXX_COMPILER_ID})
message(STATUS "Compiler Version : " ${CMAKE_CXX_COMPILER_VERSION})

file(REMOVE ${CMAKE_CACHEFILE_DIR}/CMakeCache.txt)

set(CMAKE_CXX_FLAGS " -fPIC -Wno-deprecated-declarations -fopenmp ${CMAKE_CXX_FLAGS}")

if(NOT LIB_MAJOR_VERSION)
    set(LIB_MAJOR_VERSION 1)
endif()

if(NOT OUTPUT_DIRECTORY)
    set(OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME})
endif()

file(MAKE_DIRECTORY ${OUTPUT_DIRECTORY})
set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${EXECUTABLE_OUTPUT_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${EXECUTABLE_OUTPUT_PATH})

# For Header files
set(HEADER_FILES_ROOT ${PROJECT_SOURCE_DIR}/../../../AIExpTeachingNew/CommonHeaders)
set(INCLUDE_PATH ${INCLUDE_PATH}
        ${HEADER_FILES_ROOT}/common
        ${HEADER_FILES_ROOT}
)
include(${HEADER_FILES_ROOT}/cmake/cmake_utils.cmake)
string(TIMESTAMP COMPILE_TIME %Y%m%d)
#git号
get_git_hash(GIT_HASH ${PROJECT_SOURCE_DIR})
#git分支
get_git_branch(GIT_BRANCH ${PROJECT_SOURCE_DIR})
message(STATUS "Lib source files git hash : " ${GIT_HASH})
check_git_if_clean(${HEADER_FILES_ROOT})
get_git_hash(HEADER_FILES_HASH ${HEADER_FILES_ROOT})
message(STATUS "Header source files git hash : " ${HEADER_FILES_HASH})

# ---------------------------OpenCV---------------------------
if(NOT OpenCV_SDK_NATIVE_ROOT)
    message(FATAL_ERROR "OpenCV_SDK_NATIVE_ROOT is not set, please set it to the OpenCV Android SDK native directory.")
endif()
set(OpenCV_DIR ${OpenCV_SDK_NATIVE_ROOT}/jni/abi-${ANDROID_ABI})
find_package(OpenCV REQUIRED)
set(OpenCV_INCLUDE ${OpenCV_INCLUDE_DIRS})
set(OpenCV_LIB_PATH ${OpenCV_SDK_NATIVE_ROOT}/libs/${ANDROID_ABI})

# ---------------------------FFMPEG---------------------------
if(NOT FFMPEG_ROOT)
    set(FFMPEG_ROOT ${PROJECT_SOURCE_DIR}/third_party/ffmpeg_android)    
endif()
set(FFMPEG_INCLUDE ${FFMPEG_ROOT}/${ANDROID_ABI}/include)
set(FFMPEG_LIB_PATH ${FFMPEG_ROOT}/${ANDROID_ABI}/lib)

# -----------------------------------------------------------
include_directories(${INCLUDE_PATH} ${OpenCV_INCLUDE} ${FFMPEG_INCLUDE})
link_directories(${LIB_PATH} ${OpenCV_LIB_PATH} ${FFMPEG_LIB_PATH})

set(ANDROID_VIDEOIO_LIB android_videoio)
add_subdirectory(src)
add_subdirectory(examples)
