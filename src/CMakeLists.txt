file(GLOB _tmp ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(src_all ${src_all} ${_tmp})

if (BUILD_SHARED_LIBS)
    add_library(${ANDROID_VIDEOIO_LIB} SHARED ${src_all})    
else()    
    add_library(${ANDROID_VIDEOIO_LIB} STATIC ${src_all})
endif ()
target_include_directories(${ANDROID_VIDEOIO_LIB} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/common
)

target_compile_definitions(${ANDROID_VIDEOIO_LIB} PRIVATE
        -DANDROID_VIDEOIO_LIB_NAME="${ANDROID_VIDEOIO_LIB}"
)

target_link_options(${ANDROID_VIDEOIO_LIB} PRIVATE
        -Wl,--warn-unresolved-symbols
        "-Wl,-rpath,$ORIGIN"
        #        -Wl,--no-undefined
        #        -Wl,--no-allow-shlib-undefined
)

target_link_libraries(${ANDROID_VIDEOIO_LIB} PUBLIC
		opencv_java4
                avformat
                avcodec
                avutil
                swscale
		log
)

configure_file (
        ${CMAKE_CURRENT_SOURCE_DIR}/android_videoio_version.h.in
        ${CMAKE_CURRENT_SOURCE_DIR}/android_videoio_version.h
)