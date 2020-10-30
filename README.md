# VULKAN_3D

## Getting started from git
* Cloning the repo:
```
git clone --recurse-submodules https://github.com/MoonDweller29/vulkan_3d.git
```
or
```
git clone https://github.com/MoonDweller29/vulkan_3d
cd vulkan_3d
git submodule update --init --recursive
```

### Linux build
* install [Vulkan LunarG SDK](https://vulkan.lunarg.com/). \
The best choice is to install it from package: \
(so it'll be installed in standard path and can be found by cmake)
```
sudo apt update
sudo apt install vulkan-sdk
```

* create build directory
```
mkdir build
cd build
```
* Init cmake
```
cmake ..
```
* build vk_3D:
```
make
```
* run vk_3D:
```
./vk_3D
```

### Windows build
#### Vulkan SDK
* install [Vulkan LunarG SDK](https://vulkan.lunarg.com/).
* Check that installer have set all the env variables:
	* VK\_SDK\_PATH
	* VULKAN\_SDK
	* also check that vulkan sdk path is added to PATH

#### Win32 minGW build
* create build directory
```
mkdir build
cd build
```
* Init cmake
```
cmake .. -G "MinGW Makefiles"
```
* build vk_3D:
```
mingw32-make
```
* run vk_3D:
```
.\vk_3D.exe
```

#### Windows VS build
* Choose **Open a local folder**
* Choose vulkan_3d root folder
* Wait for cmake loading
* Choose target **vk_3D**
* run

## Controls
* WASD - basic camera movement (like in a shooter :D)
* Hold down **Right** mouse button to rotate camera by mouse
* E - move up
* Q - move down
* toggle **2** to switch camera to light view
