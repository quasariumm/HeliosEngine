# Notes
## Table Of Contents
 1. [General](#General)
    1.  [Notes](#notes-1)
        1.  [Multiple graphics API's](#supporting-multiple-graphics-apis)
            1. [API abstraction](#api-abstraction)
            1. [Shader abstraction](#shader-abstraction)
            1. [Links](#usefull-links)
        1.  [DirectX 11 & 12 VS OpenGL & Vulkan](#directx-11--12-vs-opengl--vulkan)
 1. [OpenGL](#OpenGL)
    1.  [Window Creation & Managment API's](#window-creation--managment-apis)
    2.  [Links](#usefull-links-1)
 2. [OpenGL ES](#OpenGL-ES)
    1.  [Window Creation & Managment API's](#window-creation--managment-apis-1)
    2.  [Links](#usefull-links-2)
 3. [DirectX](#DirectX)
    1.  [Window Creation & Managment API's](#window-creation--managment-apis-2)
    2.  [Links](#usefull-links-3)
 3. [Vulkan](#Vulkan)
    1.  [Window Creation & Managment API's](#window-creation--managment-apis-3)
    2.  [Links](#usefull-links-4)
 3. [Metal](#Metal)
    1.  [Window Creation & Managment API's](#window-creation--managment-apis-4)
    2.  [Links](#usefull-links-5)

## General
### Notes
#### Supporting multiple graphics API's
##### API abstraction
Graphics API abstraction is the process of creating a unified, high-level interface that hides the differences among various low-level graphics APIs (like DirectX, OpenGL, Vulkan, etc.)

##### Shader abstraction
Shader abstraction is the process of creating a unified interface for managing shader code so that the same shader source can be used across multiple graphics APIs. It typically involves writing shaders in a common language and then automatically converting, compiling, and optimizing them into the native format required by each API (such as GLSL, SPIR-V, or HLSL), thereby simplifying development and increasing portability.

##### Usefull Links
https://wickedengine.net/2021/05/graphics-api-abstraction/ <br/>
https://medium.com/@sarpsenturk38/graphics-api-abstractions-part-i-4e4dac3b2483 <br/>
https://arxiv.org/pdf/2204.11025 <br/>
https://link.springer.com/chapter/10.1007/978-3-031-44751-8_4 (for full pdf of paper contact Tygo)<br/> 


#### DirectX 11 & 12 vs OpenGL & Vulkan
DirectX 11 is more similair to OpenGL while DirectX 12 is more similair to Vulkan 
(acording to this source) https://www.reddit.com/r/gameenginedevs/comments/smaitc/what_is_the_best_way_to_learn_directx/ <br /> 



## OpenGL
### Window Creation & Managment API's
- **GLFW**
- SLD
- SFML
- GLUT

### Usefull Links
https://learnopengl.com/

## OpenGL ES
### Window Creation & Managment API's
- **GLFW**
- EGL
- SDL

### Usefull Links

## DirectX
### Window Creation & Managment API's
- Built int (DXGI)

### Usefull Links
https://www.3dgep.com/category/graphics-programming/directx/ <br />
https://www.3dgep.com/category/graphics-programming/directx/directx-12/ <br />

## Vulkan
### Window Creation & Managment API's
- **GLFW**
- SDL
- QT

### Usefull Links
https://www.vulkan.org/learn#vulkan-tutorials <br />
https://vulkan-tutorial.com/ <br />
https://vkguide.dev/ <br />
https://docs.vulkan.org/tutorial/latest/00_Introduction.html <br />
https://renderdoc.org/vulkan-in-30-minutes.html <br />
https://www.jeremyong.com/c++/vulkan/graphics/rendering/2018/03/26/how-to-learn-vulkan/ <br />

## Metal
### Window Creation & Managment API's
- **GLFW**
- MetalKit

### Usefull Links
https://metaltutorial.com/ <br />