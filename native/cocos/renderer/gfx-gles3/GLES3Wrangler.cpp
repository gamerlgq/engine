/****************************************************************************
 Copyright (c) 2019-2022 Xiamen Yaji Software Co., Ltd.

 http://www.cocos.com

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
****************************************************************************/

#include "GLES3Wrangler.h"

#if defined(_WIN32) && !defined(ANDROID)
    #define WIN32_LEAN_AND_MEAN 1
    #include <windows.h>

static HMODULE libegl = NULL;
static HMODULE libgles = NULL;
static PFNGLES3WLOADPROC pfnGles3wLoad = NULL;

bool gles3wOpen() {
    libegl = LoadLibraryA("libEGL.dll");
    libgles = LoadLibraryA("libGLESv2.dll");
    return (libegl && libgles);
}

bool gles3wClose() {
    bool ret = true;
    if (libegl) {
        ret &= FreeLibrary(libegl) ? true : false;
        libegl = NULL;
    }

    if (libgles) {
        ret &= FreeLibrary(libgles) ? true : false;
        libgles = NULL;
    }

    return ret;
}

void *gles3wLoad(const char *proc) {
    void *res = nullptr;
    if (eglGetProcAddress) res = (void *)eglGetProcAddress(proc);
    if (!res) res = (void *)GetProcAddress(libegl, proc);
    return res;
}
#elif defined(__EMSCRIPTEN__)
bool gles3wOpen() { return true; }
bool gles3wClose() { return true; }
void *gles3wLoad(const char *proc) {
    return (void *)eglGetProcAddress(proc);
}
#else
    #include <dlfcn.h>

static void *libegl = nullptr;
static void *libgles = nullptr;
static PFNGLES3WLOADPROC pfnGles3wLoad = nullptr;

bool gles3wOpen() {
    libegl = dlopen("libEGL.so", RTLD_LAZY | RTLD_GLOBAL);
    libgles = dlopen("libGLESv3.so", RTLD_LAZY | RTLD_GLOBAL);
    return (libegl && libgles);
}

bool gles3wClose() {
    bool ret = true;
    if (libegl) {
        ret &= dlclose(libegl) == 0;
        libegl = nullptr;
    }

    if (libgles) {
        ret &= dlclose(libgles) == 0;
        libgles = nullptr;
    }

    return ret;
}

void *gles3wLoad(const char *proc) {
    void *res = nullptr;
    if (eglGetProcAddress) res = reinterpret_cast<void *>(eglGetProcAddress(proc));
    if (!res) res = dlsym(libegl, proc);
    return res;
}
#endif

PFNGLES3WLOADPROC pfnGLES3wLoadProc() {
    return pfnGles3wLoad;
}

bool gles3wInit() {
    if (!gles3wOpen()) {
        return false;
    }
    eglwLoadProcs(gles3wLoad);
    gles2wLoadProcs(gles3wLoad);
    gles3wLoadProcs(gles3wLoad);

    pfnGles3wLoad = gles3wLoad;
    return true;
}

bool gles3wExit() {
    return gles3wClose();
}
