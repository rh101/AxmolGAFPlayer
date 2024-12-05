Generic Animation Format
=========

Abstract
-----------------------

Library for playing Adobe Flash animations in Axmol Engine through [GAF](http://gafmedia.com).

SWF => GAF => Axmol => [iOS, Android, WinRT/UWP, Windows, Mac etc.]

Introduction
-----------------------

GAF stands for Generic Animation Format. GAF is designed to store Flash animations in an open cross platform format for further playback in a multitude of game development frameworks and devices. GAF enables artists and animators to use Flash CS for creating complex animations and seamlessly use them with various game development frameworks.

Supported Platforms
-----------------------
- Windows - Axmol v2.2.1+
- Mac OS X - Axmol v2.2.1+
- iOS - Axmol v2.2.1+
- Android - Axmol v2.2.1+
- Windows Phone 8 - Axmol v2.2.1+
 
Benefits
-----------------------
GAF Converter analyzes the bytecode of each SWF file and creates a GAF file which provides the following benefits:
### Smaller size.
**The traditional animation conversion process generates an image for each frame of the animation thus greatly increasing overall file size. However the GAF animation file stores only unique parts of the animation in a texture atlas and a highly compressed binary config file describing object positions and transformations.**
###Art requirements.
**GAF supports 99% of what can be created in Flash so your artists and animators don’t need any special planning or preparations of their SWF files before converting.**
###High performance. 
**The GAF conversion process involves numerous optimizations to increase playback performance across all platforms and frameworks. Performance of GAF runtimes is much better than that of the Flash player.**

HOWTO
-----------------------
For this folder: Add the `Library` folder to your game project.

Add the following to your `CMakeLists.txt` to link it to your application, after the `add_executable`/`add_library` section of your `CMakeLists.txt`

```
// This is assuming the Library folder has not been renamed, and it exists in the root folder of your project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Library ${PROJECT_BINARY_DIR}/app/thirdparty/GAFPlayer)
get_target_property(gafplayer_INCLUDE_DIRS gafplayer INTERFACE_INCLUDE_DIRECTORIES)
target_link_libraries(${APP_NAME} gafplayer)
target_include_directories(${APP_NAME}
    PRIVATE ${gafplayer_INCLUDE_DIRS}
)
```

For example, it would look something like this:

```
...
...
if(NOT ANDROID)
    add_executable(${APP_NAME} ${APP_SOURCES})
else()
    add_library(${APP_NAME} SHARED ${APP_SOURCES})
    # whole archive for jni
    target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)
    config_android_shared_libs("org.axmol.lib" "${CMAKE_CURRENT_SOURCE_DIR}/proj.android/app/src")
endif()

if (NOT _AX_USE_PREBUILT)
    target_link_libraries(${APP_NAME} ${_AX_CORE_LIB})
endif()

# The optional thirdparties(not dependent by engine)
if (AX_WITH_YAML_CPP)
    list(APPEND GAME_INC_DIRS "${_AX_ROOT}/3rdparty/yaml-cpp/include")
endif()

target_include_directories(${APP_NAME} PRIVATE ${GAME_INC_DIRS})

# Link GAFPlayer to project
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../Library ${PROJECT_BINARY_DIR}/app/thirdparty/GAFPlayer)
get_target_property(gafplayer_INCLUDE_DIRS gafplayer INTERFACE_INCLUDE_DIRECTORIES)
target_link_libraries(${APP_NAME} gafplayer)
target_include_directories(${APP_NAME}
    PRIVATE ${gafplayer_INCLUDE_DIRS}
)
...
...
```
Compile
Run
