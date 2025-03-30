# Notes
## Table Of Contents
 1. [General](#General)
    1.  [Notes](#notes-1)
        1.  [Multiple graphics API's](#supporting-multiple-graphics-apis)
            1. [API abstraction](#api-abstraction)
            2. [Shader abstraction](#shader-abstraction)
            3. [Links](#useful-links)
        2.  [DirectX 11 & 12 VS OpenGL & Vulkan](#directx-11--12-vs-opengl--vulkan)
 2. [OpenGL](#OpenGL)
    1.  [Window Creation & Management API's](#window-creation--management-apis)
    2.  [Shader languages](#shader-languages)
    3.  [Links](#useful-links-1)
 3. [OpenGL ES](#OpenGL-ES)
    1.  [Window Creation & Management API's](#window-creation--management-apis-1)
    2.  [Shader languages](#shader-languages-1)
    3.  [Links](#useful-links-2)
 4. [DirectX](#DirectX)
    1.  [Window Creation & Management API's](#window-creation--management-apis-2)
    2.  [Shader languages](#shader-languages-2)
    3.  [Links](#useful-links-3)
 5. [Vulkan](#Vulkan)
    1.  [Window Creation & Management API's](#window-creation--management-apis-3)
    2.  [Shader languages](#shader-languages-3)
    3.  [Links](#useful-links-4)
 6. [Metal](#Metal)
    1.  [Window Creation & Management API's](#window-creation--management-apis-4)
    2.  [Shader languages](#shader-languages-4)
    3.  [Links](#useful-links-5)
 7. [Shader languages](#shader-languages-5)
    1.  [Links](#useful-links-6)

## General
### Notes
#### Supporting multiple graphics API's
##### API abstraction
Graphics API abstraction is the process of creating a unified, high-level interface that hides the differences among various low-level graphics APIs (like DirectX, OpenGL, Vulkan, etc.)

##### Shader abstraction
Shader abstraction is the process of creating a unified interface for managing shader code so that the same shader source can be used across multiple graphics APIs. It typically involves writing shaders in a common language and then automatically converting, compiling, and optimizing them into the native format required by each API (such as GLSL, SPIR-V, or HLSL), thereby simplifying development and increasing portability.

##### Useful Links
https://wickedengine.net/2021/05/graphics-api-abstraction/  
https://medium.com/@sarpsenturk38/graphics-api-abstractions-part-i-4e4dac3b2483  
https://arxiv.org/pdf/2204.11025  
https://link.springer.com/chapter/10.1007/978-3-031-44751-8_4 (for full pdf of paper contact Tygo)   


#### DirectX 11 & 12 vs OpenGL & Vulkan
DirectX 11 is more similar to OpenGL while DirectX 12 is more similar to Vulkan 
(according to this source) https://www.reddit.com/r/gameenginedevs/comments/smaitc/what_is_the_best_way_to_learn_directx/  
DirectX 12 (Ultimate) is more modern and more performant, making it the better option to support.  


## OpenGL
### Window Creation & Management API's
- **GLFW**
- SLD
- SFML
- GLUT

### Shader languages
- GLSL
- **SPIR-V** (core since 4.6)
- Cg (_not recommended_)
- ARB assembly
- NVIDIA assembly

### Useful Links
https://learnopengl.com/  
https://www.khronos.org/opengl/wiki/Selecting_a_Shading_Language#C_for_graphics  
https://www.khronos.org/opengl/wiki/SPIR-V

## OpenGL ES
### Window Creation & Management API's
- **GLFW**
- EGL
- SDL

### Shader languages
- GLSL ES

### Useful Links

## DirectX
### Window Creation & Management API's
- Built int (DXGI)

### Shader languages
- HLSL
- **SPIR-V** (announced)

### Useful Links
https://www.3dgep.com/category/graphics-programming/directx/  
https://www.3dgep.com/category/graphics-programming/directx/directx-12/  
https://devblogs.microsoft.com/directx/directx-adopting-spir-v/  

## Vulkan
### Window Creation & Management API's
- **GLFW**
- SDL
- QT

### Shader languages
- GLSL
- HLSL
- **SPIR-V**

### Useful Links
https://www.vulkan.org/learn#vulkan-tutorials  
https://vulkan-tutorial.com/  
https://vkguide.dev/  
https://docs.vulkan.org/tutorial/latest/00_Introduction.html  
https://renderdoc.org/vulkan-in-30-minutes.html  
https://www.jeremyong.com/c++/vulkan/graphics/rendering/2018/03/26/how-to-learn-vulkan/  

## Metal
### Window Creation & Management API's
- **GLFW**
- MetalKit

### Shader languages
- MSL

### Useful Links
https://metaltutorial.com/  

## Shader languages
To make for easy development, it is convenient to not have to rewrite the shader into different languages.
So, having one language that can cross-compile is easier.  
Options:
- Slang

### Useful links
https://vulkan.org/user/pages/09.events/vulkanised-2024/vulkanised-2024-thereas-foley-nvidia.pdf  
https://github.com/shader-slang/slang  