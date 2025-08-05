//
// Created by Li Zhaoliang on 2024/10/23.
//

#ifndef ANDROID_VIDEOIO_VERSION_H
#define ANDROID_VIDEOIO_VERSION_H

#include <cstdio>

#define COMPILE_TIME "20250805"
#define GIT_BRANCH "master"
#define GIT_HASH "2d07258"

static inline void print_version()
{
    printf("Compile time: %s\n", COMPILE_TIME);
    printf("Ndk videoio name: %s\n", ANDROID_VIDEOIO_LIB_NAME);
    printf("Git branch: %s\n", GIT_BRANCH);
    printf("Git hash: %s\n", GIT_HASH);
}

#endif //ANDROID_VIDEOIO_VERSION_H
