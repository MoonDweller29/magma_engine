# VULKAN_3D

## Getting started from git
* Cloning the repo:
```
git clone https://github.com/MoonDweller29/vulkan_3d
cd vulkan_3d
```
* Fetching project dependencies:
```
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
* Init cmake \
(it will automatically load minimal required data):
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

### Win32 minGW build
* install [Vulkan LunarG SDK](https://vulkan.lunarg.com/).

TODO:

### Win32 VS build
* install [Vulkan LunarG SDK](https://vulkan.lunarg.com/).

TODO:

## Controls
* WASD - basic camera movement
* Hold down **Right** mouse button to rotate camera by mouse
* E - move up
* Q - move down
* toggle **2** to switch camera to light view