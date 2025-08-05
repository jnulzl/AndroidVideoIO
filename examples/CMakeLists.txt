add_executable(android_videoio_demo
    android_videoio_demo.cpp
)

target_link_libraries(android_videoio_demo
    PRIVATE ${ANDROID_VIDEOIO_LIB}
)
target_include_directories(android_videoio_demo PRIVATE 
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/common
)