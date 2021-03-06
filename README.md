
![logo](bg2-logo-web.png)

# bg2 engine - C++ API

bg2e is a graphic engine oriented to the creation of graphic applications. It is designed to cover some features that graphic engines for videogames do not have:

- Native integration with operating system APIs, especially with user interface elements.
- Load and store scenes and 3D models at runtime: no need to pack the scenes together with the executable application.
- Lightweight libraries: you don't need to distribute hundreds of megabytes of resources with the application.

bg2 engine is available in different APIs:

- JavaScript/WebGL
- C++ (this repository)
- Unreal C++

Please note that bg2 engine is not suitable for all projects. To help you decide whether to continue or quit, here are some tips:

- High level API: bg2 engine is not as flexible as other Open Source graphics engines for applications. The characteristics of 3D models and materials are quite strict. In this sense it resembles more to other graphic engines for videogames. 3D models must have normal and one or two texture channels. The materials are based on the PBR model and are defined using scalar, color or texture values for the typical properties of these materials (albedo, normal, roughness, metallicity, ambient occlussion, displacement map).
- Integrated scene editor: one way or another, bg2 engine will always include an integrated scene editor to facilitate the creation of resources for loading 3D models. Currently, this editor is Open Source and has been created using the JavaScript/WebGL API through Electron.js. In the future there may be other native and commercial options, but the intention is that there is always at least one Open Source version. Obviously, the scene editor is created using bg2 engine, so if you dare you are free to create your own editor.
- Multiple APIs for each use: with bg2 engine you can create native applications for various environments, but keep in mind that each of them has its own API. You can't always program a single application and compile it on different platforms, and the APIs are designed to work in each environment following the recommendations of each language. For example, scene elements don't work the same way in the Swift API as they do in the JavaScript API. The advantage of this approach is that we get the maximum possible integration in each platform.
- Not all bg2 engine APIs have the same features: for example, the Swift APIs will have more possibilities to work with sound, because we can use the framework AVFoundation and Core Audio in a simple way. In the same way, with JavaScript APIs it is easier to use vector graphics as texture, since the web browser allows us to load SVG files, or even generate textures dynamically using a canvas. The basic features that can be created with the editor will be very similar, but when choosing the API you are going to use, you must take into account what you are going to use it for.

# The C++ API

This repository implements the C++ API for bg2 engine. Use this version of the engine if

- You want to make a multiplatform application with the maximum performance possible, especially in desktop environments (Windows, macOS or Linux).
- You need to use C++ in your application.
- It is not necessary that your application works on smartphones: it is possible to use this version as a base for an Android application (Android support is in the roadmap, but not yet tested), but if your target is iOS it will be much better to use the specific version for Swift.

## Installation

### Requirements and dependencies

bg2 engine is divided into different libraries, and not all of them have the same requirements, but basically you will need:

- BGFX: graphical API independence layer.
- GLFW: window system.
- Node.js: to execute the automatic dependency download system.

You can use the automatic dependency download system to get BGFX and GLFW precompiled version



## Usage

### macOS Xcode

Configure search paths

Header search paths:

```
$(BG2E_DIR)/include
$(BG2E_DIR)/deps/glfw-3.3/osx/include
$(BG2E_DIR)/deps/bgfx/osx/include
```

You'll also need to link your app to bgfx and glfw: 

- libbimg_decodeRelease.a
- libbgfxRelease.a
- libbxRelease.a
- libbimgRelease.a
- libglfw3.a

Library search paths:

```
$(BG2E_DIR)/deps/glfw-3.3/osx/lib
$(BG2E_DIR)/deps/bgfx/osx/lib
```

And you'll need also to link to the following macOS frameworks:

- Cocoa.framework
- OpenGL.framework
- CoreVideo.framework
- IOKit.framework
- Metal.framework
- MetalKit.framework
- QuartzCore.framework

To avoid glfw documentation warning like `Empty paragraph passed to '@sa' command`, disable `Documentation comments` warnings in `Build Settings`

### Windows Visual Studio

Search paths: add the paths to locate the bg2e library and also its dependencies (Project settings > VC++ Directories)

Include Directories:

- $(BG2E_DIR)\include
- $(BG2E_DIR)\deps\glfw-3.3\win64\include
- $(BG2E_DIR)\deps\bgfx\win64\include

Library Directories:

- $(BG2E_DIR)\build\$(Configuration)
- $(BG2E_DIR)\deps\glfw-3.3\win64\lib
- $(BG2E_DIR)\deps\bgfx\win64\lib

You can use the $(Configuration) variable in Visual Studio to reference the appropiate configuration (Debug or Release).

Linker: link with bg2e library and its dependencies.

- bgfx$(Configuration).lib
- bimg_decode$(Configuration).lib
- bimg$(Configuration).lib
- bx$(Configuration).lib
- glfw3.lib
- bg2e.lib

Other settings: To link with bg2e as a static library, you must to set, in C/C++ > Code Generation > Runtime Library:

- In Debug configuration: Multi-threaded Debug (/MTd)
- In Release configuration: Multi-threaded (/MT)


## Basic usage

registerXXX()/unregisterXXX():

A.registerB(B): Object A will be responsible for managing the memory of B
B.unregisterB(): Object A will no longer manage B's memory. This function returns a pointer to the managed object, which can be used to obtain the responsibility of managing the memory, or nullptr if nothing is being managed by A

