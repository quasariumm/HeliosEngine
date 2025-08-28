# Helios Engine
An engine made to be easy to use and compatible with a large group of graphics APIs and rendering techniques.

The engine uses the latest commit of the Tracy Profiler.

## Installation

To run the engine, you are required to install a set of packages. They are listed below
along with their installation steps

### FFmpeg

You can download it from their [download page](https://ffmpeg.org/download.html) 
or via your system's package manager:

#### Windows
```commandline
winget install Gyan.FFmpeg.Shared
```
Additional note for people using Bundled MinGW on any JetBrains IDE:  
After installing the FFmpeg package, you might have to copy the `.lib/.dll/.dll.a` library and the include directory files from  
`%AppData%\Local\Microsoft\WinGet\Packages\Gyan.FFmpeg.Shared_...\ffmpeg-<version>-full_build-shared\[lib|include]`  
to  
`%AppData%\Local\Programs\CLion\bin\mingw\[lib|include]`

#### Linux / WSL / MySYS / MinGW
Depending on the distro, use your package manager to install the package (use sudo if necessary).  
Commands for some common package managers are listed below
```shell
sudo apt install ffmpeg libavcodec-dev libavformat-dev libavutil-dev libswscale-dev
```
```shell
sudo dnf install ffmpeg ffmpeg-devel
```
```shell
sudo pacman -S ffmpeg
```
```shell
sudo yum install epel-release && sudo yum install ffmpeg ffmpeg-devel
```