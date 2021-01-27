/**
 * @file textureDump.h
 * @author Nastormo
 * @brief Header file that contains texture dump class
 * @version 0.1
 * @date 2021-01-24
 */
#pragma once


#include "vk/buffer.h"
#include "vk/texture.h"
#include "vk/logicalDevice.h"
#include "vk/commandBuffer.h"

class TextureDump {
private:

public:
    void save(LogicalDevice &device, Texture &texture);

};