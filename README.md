# Helios Engine
An engine made to be easy to use and compatible with a large group of graphics APIs and rendering techniques.

The engine uses the latest commit of the Tracy Profiler.

## Installing OpenCL
### Windows
_This requires the following things to be installed: Visual Studio Build Tools, Git and CMake_  
To install the OpenCL SDK, we recommend to install the latest version from the official GitHub repository.
Because you only need the installed version, you can put the SDK repo in the TMP folder.
Run the following commands in a terminal to install it automatically 
(you need to launch it as Administrator, since the SDK's files will go in the `Program Files` directory):  
**NOTE: You can ignore error messages regarding already existing files or directories**
#### Powershell
```powershell
cd $env:TEMP
git clone --recursive https://github.com/KhronosGroup/OpenCL-SDK.git
cmake -G "Visual Studio 17 2022" -A x64 -T v143 -D CMAKE_INSTALL_PREFIX=./OpenCL-SDK/install -B ./OpenCL-SDK/build -S ./OpenCL-SDK
cmake --build OpenCL-SDK/build --config Release --target install -- /m /v:minimal
cd .\OpenCL-SDK\install
ren include inc
cd lib
mkdir x64
mv .\* x64\
cd ..
mkdir $env:programfiles\OpenCL\common
mv .\* $env:programfiles\OpenCL\common
```
#### Command Prompt
```batch
cd %TEMP%
git clone --recursive https://github.com/KhronosGroup/OpenCL-SDK.git
cmake -G "Visual Studio 17 2022" -A x64 -T v143 -D CMAKE_INSTALL_PREFIX=./OpenCL-SDK/install -B ./OpenCL-SDK/build -S ./OpenCL-SDK
cmake --build OpenCL-SDK/build --config Release --target install -- /m /v:minimal
cd .\OpenCL-SDK\install
ren include inc
cd lib
mkdir x64
mv .\* x64\
cd ..
mkdir %programfiles%\OpenCL\common
mv .\* %programfiles%\OpenCL\common
```