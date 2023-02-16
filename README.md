
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

- GLFW: window system.
- GLM: math library
- stb_image
- bg2io
- VulkanSDK version 1.3

## Usage

### macOS Xcode

#### Setup script

TODO: create a script to setup the dependencies automatically, for example:

```sh
% setup-deps.sh ~/VulkanSDK/1.3.239.0
```

#### Setup VulkanSDK

The first thing we have to do is to download VulkanSDK from the web and install it. It doesn't matter where we install it, but it is important that you point the path to the SDK. The default installation leaves the SDK in the location ~/VulkanSDK/1.3.xxx.x (where 1.3.xxx.x is the SDK version). The path we are interested in is the path to the folder that has the version. What we will do is create a symbolic link from this folder to the `[bg2e-cpp-repo]/deps/VulkanSDK` folder.

For example, if you have installed VulkanSDK in `~/VulkanSDK/1.3.239.0` you would have to create the symlink like this:

```sh
% cd [path-to-bg2e-cpp-repo]
% ln -s ~/VulkanSDK/1.3.239.0 deps/VulkanSDK
%
% ls -l deps
total 0
lrwxr-xr-x  1 your-user  staff  35 16 feb 19:52 VulkanSDK -> /Users/your-user/VulkanSDK/1.3.239.0
drwxr-xr-x  2 your-user  staff  64 16 feb 19:53 glm
... other dependencies
```

This way, the Xcode project will be able to find all VulkanSDK resources, regardless of the particular version you have installed and the location you have chosen.

#### bg2io

This library is part of the bg2 engine library package, and is responsible for reading and writing files in binary format native to bg2 engine. It is composed of a series of C and C++ files. The Xcode project automatically includes the references to the bg2io files, you only have to download the repository in the deps folder:

```sh
% cd [path-to-bg2e-cpp-repo]/deps
% git clone https://github.com/ferserc1/bg2-io
```

#### Header only libraries

bg2e-cpp has other dependencies that are in the form of header files. These dependencies are included directly in the `deps` directory, so you do not have to download them:

- GLM
- stb_image

To avoid glfw documentation warning like `Empty paragraph passed to '@sa' command`, disable `Documentation comments` warnings in `Build Settings`

### Windows Visual Studio


