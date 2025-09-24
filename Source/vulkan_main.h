#pragma once
#include "baseTypes.h"
#include "fileio.h"
#include <cstring>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_win32.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

const int32 MAX_FRAMES_IN_FLIGHT = 2;
const int32 MAP_WIDTH = 25;
const int32 MAP_HEIGHT = 13;
const real32 TILE_SIZE = 32;

inline const char *validationLayers[] = {
    "VK_LAYER_KHRONOS_validation",
    // "VK_LAYER_LUNARG_crash_diagnostic"
};
inline const char *win32Extentions[] = {"VK_KHR_win32_surface", VK_KHR_SURFACE_EXTENSION_NAME};
inline const char *deviceExtentions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
    VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
    VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME,
    VK_KHR_MULTIVIEW_EXTENSION_NAME,
    VK_KHR_MAINTENANCE2_EXTENSION_NAME};

struct UniformBufferObjext {
    Mat4 model;
    Mat4 view;
    Mat4 proj;
    real32 time;
};

struct Texture {
    VkTexture texture;
    VkDeviceMemory memory;
};

struct VulkanState {
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;

    uint32 graphicsFamily;
    uint32 presentFamily;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkExtent2D extent;

    VkSurfaceKHR surface;
    VkSurfaceFormatKHR surfaceFormat;

    VkSwapchainKHR swapchain;
    VkImage *swapchainImages;
    uint32 swapchainImagesCount;
    VkImageView *swapChainImageViews;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    VkPipeline graphicsPipeline2;

    VkCommandPool commandPool;
    VkSemaphore presentCompleteSemaphore[MAX_FRAMES_IN_FLIGHT];
    VkSemaphore renderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
    VkFence inFlightFences[MAX_FRAMES_IN_FLIGHT]{};
    VkCommandBuffer commandBuffer[MAX_FRAMES_IN_FLIGHT]{};

    const Vertex vertices[4]{
        {{-1.f, -1.f}, {0.f, 1.f, .5f}  },
        {{1.f, -1.f},  {0.f, .25f, .5f} },
        {{1.f, 1.f},   {0.f, .25f, 1.0f}},
        {{-1.f, 1.f},  {1.f, 1.f, 1.f}  },
    };
    const uint16 indices[6]{0, 1, 2, 2, 3, 0};

    Vertex vertices2[(MAP_WIDTH) * (MAP_HEIGHT) * 6]{0};

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    VkBuffer vertexBuffer2;
    VkDeviceMemory vertexBufferMemory2;

    Texture texture;
};

void generateTiles(Vertex *vertices, float tileSize, Vector2i mapSize) {

    // the origin of each tile is the top left
    int32 index = 0;
    for (int32 y = 0; y < mapSize.y; y++) {
        for (int32 x = 0; x < mapSize.x; x++) {
            vertices[index++] = {
                {x * tileSize, y * tileSize},
                {0, 0, 0},
                {0, 0}
            };
            vertices[index++] = {
                {(x + 1) * tileSize, y * tileSize},
                {0, 0, 0},
                {1, 0}
            };
            vertices[index++] = {
                {(x + 1) * tileSize, (y + 1) * tileSize},
                {0, 0, 0},
                {1, 1}
            };

            vertices[index++] = {
                {(x + 1) * tileSize, (y + 1) * tileSize},
                {0, 0, 0},
                {1, 1}
            };
            vertices[index++] = {
                {(x)*tileSize, (y + 1) * tileSize},
                {0, 0, 0},
                {0, 1}
            };
            vertices[index++] = {
                {x * tileSize, y * tileSize},
                {0, 0, 0},
                {0, 0}
            };
        }
    }
};

static VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}

static Array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
    Array<VkVertexInputAttributeDescription, 3> attributeDescriptions;
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, uv);
    return attributeDescriptions;
}

bool MakeSurface(VulkanState &vkState);
VkExtent2D GetClientExtents();
bool createGraphicsPipeline(VulkanState *vkState, VkPipeline *graphicsPipeline, const char *vertex_file, const char *fragment_file);

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    uint32 formatsCount;
    VkSurfaceFormatKHR *formats;
    uint32 presentModesCount;
    VkPresentModeKHR *presentModes;
};

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
    SwapChainSupportDetails details{};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &(details.formatsCount), nullptr);
    if (details.formatsCount != 0) {
        details.formats = AllocateType(VkSurfaceFormatKHR, details.formatsCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.formatsCount, details.formats);
    }

    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &(details.presentModesCount), nullptr);
    if (details.presentModesCount != 0) {
        details.presentModes = AllocateType(VkPresentModeKHR, details.formatsCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &(details.presentModesCount), details.presentModes);
    }

    return details;
}

uint32 findMemoryType(VulkanState *vkState, uint32 typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vkState->physicalDevice, &memProperties);
    for (uint32 i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    return -1;
}

bool createBuffer(VulkanState *vkState, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *bufferMemory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.usage = usage;
    bufferInfo.size = size;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vkCreateBuffer(vkState->device, &bufferInfo, nullptr, buffer);

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(vkState->device, *buffer, &memoryRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(vkState, memoryRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(vkState->device, &allocInfo, nullptr, bufferMemory) != VK_SUCCESS) {
        DebugLog(L"Failed to create buffer!");
        return false;
    }
    vkBindBufferMemory(vkState->device, *buffer, *bufferMemory, 0);
    return true;
}

void copyBuffer(VulkanState *vkState, VkBuffer &srcBuffer, VkBuffer &dstBuffer, VkDeviceSize size) {

    VkCommandBufferAllocateInfo commandBufferInfo{};
    commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferInfo.commandBufferCount = 1;
    commandBufferInfo.commandPool = vkState->commandPool;
    commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    VkCommandBuffer copyCommandBuffer;
    vkAllocateCommandBuffers(vkState->device, &commandBufferInfo, &copyCommandBuffer);

    VkCommandBufferBeginInfo copyCommandBufferBeginInfo{};
    copyCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    copyCommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(copyCommandBuffer, &copyCommandBufferBeginInfo);
    VkBufferCopy bufferCopy{};
    bufferCopy.size = size;

    vkCmdCopyBuffer(copyCommandBuffer, srcBuffer, dstBuffer, 1, &bufferCopy);
    vkEndCommandBuffer(copyCommandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &copyCommandBuffer;

    vkQueueSubmit(vkState->graphicsQueue, 1, &submitInfo, nullptr);
    vkQueueWaitIdle(vkState->graphicsQueue);
}

bool createTextureImage(VulkanState *vkState) {
    int texWidth, texHeight, texChannels;
    stbi_uc *pixels = stbi_load("textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        DebugLog(L"failed to load texture image!");
        return false;
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;
    createBuffer(vkState, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingMemory);

    void *data;
    vkMapMemory(vkState->device, stagingMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, imageSize);
    vkUnmapMemory(vkState->device, stagingMemory);
    stbi_image_free(pixels);

    VkFormat format;

    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = format;
    imageCreateInfo.extent.width = texWidth;
    imageCreateInfo.extent.height = texHeight;
    imageCreateInfo.extent.depth = 1;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    // TODO: finish this
    // vk::ImageCreateInfo imageInfo({}, vk::ImageType::e2D, format, {width, height, 1}, 1, 1, vk::SampleCountFlagBits::e1, tiling, usage, vk::SharingMode::eExclusive, 0);
}

bool createVertexBuffers(VulkanState *vkState) {

    VkDeviceSize vertexBuffer1Size = sizeof(vkState->vertices);
    VkDeviceSize vertexBuffer2Size = sizeof(vkState->vertices2);
    VkDeviceSize stagingBufferSize = max(vertexBuffer1Size, vertexBuffer2Size);

    createBuffer(vkState, vertexBuffer1Size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vkState->vertexBuffer, &vkState->vertexBufferMemory);
    createBuffer(vkState, vertexBuffer2Size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vkState->vertexBuffer2, &vkState->vertexBufferMemory2);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;
    createBuffer(vkState, stagingBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingMemory);

    void *data;
    vkMapMemory(vkState->device, stagingMemory, 0, vertexBuffer1Size, 0, &data);
    memcpy(data, vkState->vertices, vertexBuffer1Size);
    copyBuffer(vkState, stagingBuffer, vkState->vertexBuffer, vertexBuffer1Size);

    vkMapMemory(vkState->device, stagingMemory, 0, vertexBuffer2Size, 0, &data);
    memcpy(data, vkState->vertices2, vertexBuffer2Size);
    copyBuffer(vkState, stagingBuffer, vkState->vertexBuffer2, vertexBuffer2Size);

    vkUnmapMemory(vkState->device, stagingMemory);
    vkDestroyBuffer(vkState->device, stagingBuffer, nullptr);
    return true;
}
bool createIndexBuffer(VulkanState *vkState) {
    VkDeviceSize indexBufferSize = sizeof(vkState->indices);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    createBuffer(vkState, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    void *data;
    vkMapMemory(vkState->device, stagingBufferMemory, 0, indexBufferSize, 0, &data);
    memcpy(data, vkState->indices, (size_t)indexBufferSize);
    vkUnmapMemory(vkState->device, stagingBufferMemory);

    createBuffer(vkState, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vkState->indexBuffer, &vkState->indexBufferMemory);

    copyBuffer(vkState, stagingBuffer, vkState->indexBuffer, indexBufferSize);
    vkDestroyBuffer(vkState->device, stagingBuffer, nullptr);
    return true;
}

VkShaderModule createShaderModule(VkDevice device, DebugReadFileResult file) {

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = file.Size;
    createInfo.pCode = reinterpret_cast<const uint32_t *>(file.Data);
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        DebugLog(L"failed to create shader module");
    }
    return shaderModule;
}
void createSwapChain(VulkanState *vkState) {
    vkState->extent = GetClientExtents();
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(vkState->physicalDevice, vkState->surface);

    for (uint32 formatIndex = 0; formatIndex < swapChainSupport.formatsCount; formatIndex++) {
        auto availableFormat = swapChainSupport.formats[formatIndex];
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            vkState->surfaceFormat = availableFormat;
        }
    }

    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

    uint32 imageCount = 2;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }
    uint32 queueFamilyIndices[] = {vkState->graphicsFamily, vkState->presentFamily};

    VkSwapchainCreateInfoKHR swapchainCreateInfo{};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = vkState->surface;
    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageFormat = vkState->surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = vkState->surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = vkState->extent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    // note: for image usage we will render directly to the image but if we want to do some kind of
    // post proccessing or ui we will have to use VK_IMAGE_USAGE_TRANSFER_DST_BIT and use a memory operation
    // to transfer the image to the swapchain
    if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
        // this is not supported for now
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 0;     // Optional
        swapchainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    swapchainCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    vkCreateSwapchainKHR(vkState->device, &swapchainCreateInfo, nullptr, &vkState->swapchain);
    vkGetSwapchainImagesKHR(vkState->device, vkState->swapchain, &vkState->swapchainImagesCount, nullptr);
    vkState->swapchainImages = AllocateType(VkImage, vkState->swapchainImagesCount);
    vkGetSwapchainImagesKHR(vkState->device, vkState->swapchain, &vkState->swapchainImagesCount, vkState->swapchainImages);
}

void createImageViews(VulkanState *vkState) {
    vkState->swapChainImageViews = AllocateType(VkImageView, vkState->swapchainImagesCount);
    for (uint32 viewIndex = 0; viewIndex < vkState->swapchainImagesCount; viewIndex++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = vkState->swapchainImages[viewIndex];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = vkState->surfaceFormat.format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(vkState->device, &createInfo, nullptr, &vkState->swapChainImageViews[viewIndex]) != VK_SUCCESS) {
            DebugLog(L"Failed to create image view");
        }
    }
}

void cleanupSwapChain(VulkanState *vkState) {

    for (uint32 imageViewIndex = 0; imageViewIndex < vkState->swapchainImagesCount; imageViewIndex++) {
        vkDestroyImageView(vkState->device, vkState->swapChainImageViews[imageViewIndex], nullptr);
    }

    vkDestroySwapchainKHR(vkState->device, vkState->swapchain, nullptr);
}

void recreateSwapChain(VulkanState *vkState) {
    vkDeviceWaitIdle(vkState->device);

    cleanupSwapChain(vkState);

    createSwapChain(vkState);
    createImageViews(vkState);
}

void createCommandPool(VulkanState *vkState) {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = vkState->graphicsFamily;
    if (vkCreateCommandPool(vkState->device, &poolInfo, nullptr, &vkState->commandPool) != VK_SUCCESS) {
        DebugLog(L"failed to create command pool!");
    }
}

inline bool createCommandBuffer(VulkanState *vkState) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = vkState->commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    if (vkAllocateCommandBuffers(vkState->device, &allocInfo, vkState->commandBuffer) != VK_SUCCESS) {
        DebugLog(L"failed to allocate command buffers!");
        return true;
    }
    return false;
}

bool createSyncObjects(VulkanState *vkState) {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int32 Index = 0; Index < MAX_FRAMES_IN_FLIGHT; Index++) {
        if (vkCreateSemaphore(vkState->device, &semaphoreInfo, nullptr, &vkState->presentCompleteSemaphore[Index]) != VK_SUCCESS ||
            vkCreateSemaphore(vkState->device, &semaphoreInfo, nullptr, &vkState->renderFinishedSemaphores[Index]) != VK_SUCCESS ||
            vkCreateFence(vkState->device, &fenceInfo, nullptr, &vkState->inFlightFences[Index]) != VK_SUCCESS) {
            DebugLog(L"failed to create semaphores!");
            return true;
        }
    }
    return false;
}

bool createDescriptorSetLayout(VulkanState *vkState) {

    return true;
    // TODO: finish this
}
bool createGraphicsPipeline(VulkanState *vkState, VkPipeline *graphicsPipeline,
                            const char *vertex_file = "Shaders/triangle/shader_vert.spv",
                            const char *fragment_file = "Shaders/triangle/shader_frag.spv") {
    auto vertShaderCode = DebugReadFile(vertex_file);
    auto fragShaderCode = DebugReadFile(fragment_file);

    auto fragShaderModule = createShaderModule(vkState->device, fragShaderCode);
    auto vertShaderModule = createShaderModule(vkState->device, vertShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    auto bindingDescription = getBindingDescription();
    auto attributeDescriptions = getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = length(attributeDescriptions);
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data; // Optional

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT,
                                      VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = length(dynamicStates);
    dynamicState.pDynamicStates = dynamicStates;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f;          // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;          // Optional
    multisampling.pSampleMask = nullptr;            // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE;      // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;             // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;             // Optional

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    VkPipelineLayout pipelineLayout{};
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;            // Optional
    pipelineLayoutInfo.pSetLayouts = nullptr;         // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    if (vkCreatePipelineLayout(vkState->device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        DebugLog(L"failed to create pipeline layout");
        return false;
    }

    VkAttachmentReference2 colorAttachmentRef{};
    colorAttachmentRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription2 subpass{};
    subpass.sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2;
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkAttachmentDescription2 colorAttachment{};
    colorAttachment.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
    colorAttachment.format = vkState->surfaceFormat.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkSubpassDependency2 dependency{};
    dependency.sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkPipelineRenderingCreateInfo pipelineRenderingCreateInfo{};
    pipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    pipelineRenderingCreateInfo.colorAttachmentCount = 1;
    pipelineRenderingCreateInfo.pColorAttachmentFormats = &vkState->surfaceFormat.format;

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = &pipelineRenderingCreateInfo;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = VK_NULL_HANDLE;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1;              // Optional

    if (vkCreateGraphicsPipelines(vkState->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, graphicsPipeline) != VK_SUCCESS) {
        DebugLog(L"failed to create graphics pipeline!");
    }

    return true;
}

inline bool InnitVulkan(VulkanState *vkState) {
    Vector2i mapSize = {MAP_WIDTH, MAP_HEIGHT};
    generateTiles(vkState->vertices2, TILE_SIZE, mapSize);
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_4;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = length(win32Extentions);
    createInfo.ppEnabledExtensionNames = win32Extentions;
    createInfo.enabledLayerCount = length(validationLayers);
    createInfo.ppEnabledLayerNames = validationLayers;

    if (vkCreateInstance(&createInfo, nullptr, &vkState->instance) != VK_SUCCESS) {
        DebugLog(L"Unable to create vulkan instance");
        return false;
    }

    uint32 deviceCount = 0;
    vkEnumeratePhysicalDevices(vkState->instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        DebugLog(L"No valid vulkan device found");
        return false;
    }
    VkPhysicalDevice devices[7];
    vkEnumeratePhysicalDevices(vkState->instance, &deviceCount, devices);
    vkState->physicalDevice = devices[0];

    uint32 queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(vkState->physicalDevice, &queueFamilyCount, nullptr);
    VkQueueFamilyProperties *queueFamilyProperties = AllocateType(VkQueueFamilyProperties, queueFamilyCount);

    vkGetPhysicalDeviceQueueFamilyProperties(vkState->physicalDevice, &queueFamilyCount, queueFamilyProperties);
    for (uint32 familyIndex = 0; familyIndex < queueFamilyCount; familyIndex++) {
        if (queueFamilyProperties[familyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            vkState->graphicsFamily = familyIndex;
        }
        VkBool32 presentSupport = vkGetPhysicalDeviceWin32PresentationSupportKHR(vkState->physicalDevice, familyIndex);
        if (presentSupport) {
            vkState->presentFamily = familyIndex;
            if (vkState->graphicsFamily != -1) {
                break;
            }
        }
    }
    if (vkState->graphicsFamily == -1) {
        DebugLog(L"Unable to find graphics family");
        return false;
    }
    if (vkState->presentFamily == -1) {
        DebugLog(L"Unable to find present family");
        return false;
    }

    VkDeviceQueueCreateInfo queueCreateInfos[2]{};
    {
        float queuePriority = 1.0f;
        VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.queueFamilyIndex = vkState->graphicsFamily;
        deviceQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos[0] = deviceQueueCreateInfo;
    }
    if (vkState->graphicsFamily != vkState->presentFamily) {

        float queuePriority = 1.0f;
        VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.queueFamilyIndex = vkState->presentFamily;
        deviceQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos[1] = deviceQueueCreateInfo;
    }

    constexpr VkPhysicalDeviceSynchronization2Features synchronization2_feature{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
        .synchronization2 = VK_TRUE};
    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_feature{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
        .pNext = (void *)&synchronization2_feature,
        .dynamicRendering = VK_TRUE

    };

    VkPhysicalDeviceFeatures deviceFeatures{};
    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = &dynamic_rendering_feature;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos;
    deviceCreateInfo.queueCreateInfoCount = vkState->graphicsFamily == vkState->presentFamily ? 1 : 2;
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = length(deviceExtentions);
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtentions;

    if (vkCreateDevice(vkState->physicalDevice, &deviceCreateInfo, nullptr, &vkState->device) != VK_SUCCESS) {
        DebugLog(L"Unable to create vulkan device");
        return false;
    }
    vkGetDeviceQueue(vkState->device, vkState->graphicsFamily, 0, &vkState->graphicsQueue);
    vkGetDeviceQueue(vkState->device, vkState->presentFamily, 0, &vkState->presentQueue);

    MakeSurface(*vkState);
    createSwapChain(vkState);

    createImageViews(vkState);
    createDescriptorSetLayout(vkState);
    createGraphicsPipeline(vkState, &vkState->graphicsPipeline);
    createGraphicsPipeline(vkState, &vkState->graphicsPipeline2,
                           "Shaders/RoundedCorners/shader_vert.spv",
                           "Shaders/RoundedCorners/shader_frag.spv");
    createCommandPool(vkState);
    createVertexBuffers(vkState);
    createIndexBuffer(vkState);
    createCommandBuffer(vkState);
    createSyncObjects(vkState);
    return true;
}

void transitionImageLayout(VulkanState *vkState, uint32 imageIndex,
                           VkImageLayout oldLayout, VkImageLayout newLayout,
                           VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask,
                           VkPipelineStageFlags2 srcStageMask, VkPipelineStageFlags2 dstStageMask) {
    VkImageMemoryBarrier2 imageMemoryBarrier{};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    imageMemoryBarrier.oldLayout = oldLayout;
    imageMemoryBarrier.newLayout = newLayout;
    imageMemoryBarrier.srcAccessMask = srcAccessMask;
    imageMemoryBarrier.dstAccessMask = dstAccessMask;
    imageMemoryBarrier.srcStageMask = srcStageMask;
    imageMemoryBarrier.dstStageMask = dstStageMask;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.image = vkState->swapchainImages[imageIndex];
    imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    imageMemoryBarrier.subresourceRange.levelCount = 1;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.layerCount = 1;

    VkDependencyInfo dependencyInfo{};
    dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    dependencyInfo.dependencyFlags = {};
    dependencyInfo.imageMemoryBarrierCount = 1;
    dependencyInfo.pImageMemoryBarriers = &imageMemoryBarrier;

    vkCmdPipelineBarrier2(vkState->commandBuffer[imageIndex], &dependencyInfo);
}

bool RecordCommandBuffer(VulkanState *vkState, uint32 currentFrame, uint32 imageIndex) {

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(vkState->commandBuffer[currentFrame], &beginInfo) != VK_SUCCESS) {
        DebugLog(L"failed to begin recording command buffer!");
        return false;
    }
    transitionImageLayout(vkState,
                          imageIndex,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                          {},
                          VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                          VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
                          VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);

    VkClearValue clearValue{};
    clearValue.color = {0.0f, 0.0f, 0.0f, 1.0f};

    VkRenderingAttachmentInfo renderingAttachmentInfo{};
    renderingAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    renderingAttachmentInfo.imageView = vkState->swapChainImageViews[imageIndex];
    renderingAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    renderingAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    renderingAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    renderingAttachmentInfo.clearValue = clearValue;

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea.offset = {0, 0};
    renderingInfo.renderArea.extent = vkState->extent;
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &renderingAttachmentInfo;
    vkCmdBeginRendering(vkState->commandBuffer[currentFrame], &renderingInfo);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)vkState->extent.width;
    viewport.height = (float)vkState->extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(vkState->commandBuffer[currentFrame], 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = vkState->extent;
    vkCmdSetScissor(vkState->commandBuffer[currentFrame], 0, 1, &scissor);

    VkDeviceSize offsets[] = {0};

    vkCmdBindPipeline(vkState->commandBuffer[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, vkState->graphicsPipeline);
    vkCmdBindVertexBuffers(vkState->commandBuffer[currentFrame], 0, 1, &vkState->vertexBuffer, offsets);
    vkCmdBindIndexBuffer(vkState->commandBuffer[currentFrame], vkState->indexBuffer, 0, VK_INDEX_TYPE_UINT16);
    vkCmdDrawIndexed(vkState->commandBuffer[currentFrame], length(vkState->indices), 1, 0, 0, 0);

    vkCmdBindPipeline(vkState->commandBuffer[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, vkState->graphicsPipeline2);
    vkCmdBindVertexBuffers(vkState->commandBuffer[currentFrame], 0, 1, &vkState->vertexBuffer2, offsets);
    vkCmdDraw(vkState->commandBuffer[currentFrame], length(vkState->vertices2), 1, 0, 0);

    vkCmdEndRendering(vkState->commandBuffer[currentFrame]);
    transitionImageLayout(vkState,
                          imageIndex,
                          VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                          VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                          VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                          {},
                          VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                          VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT);
    vkEndCommandBuffer(vkState->commandBuffer[currentFrame]);
    return true;
}

bool render(VulkanState *vkState, uint32 &currentFrame) {

    while (vkWaitForFences(vkState->device, 1, &vkState->inFlightFences[currentFrame], VK_TRUE, UINT64_MAX) == VK_TIMEOUT)
        ;
    // BUG: small bug with semaphores studdy vulkan spec
    uint32 imageIndex;
    VkResult result = vkAcquireNextImageKHR(vkState->device, vkState->swapchain, UINT64_MAX, vkState->presentCompleteSemaphore[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        auto extensts = GetClientExtents();
        if (extensts.width == 0 || extensts.height == 0) {
            return true;
        }
        recreateSwapChain(vkState);
        return true;
    }
    vkResetFences(vkState->device, 1, &vkState->inFlightFences[currentFrame]);
    vkResetCommandBuffer(vkState->commandBuffer[currentFrame], 0);
    RecordCommandBuffer(vkState, currentFrame, imageIndex);

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &(vkState->presentCompleteSemaphore[currentFrame]);
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vkState->commandBuffer[currentFrame];
    VkSemaphore signalSemaphores[] = {vkState->renderFinishedSemaphores[imageIndex]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    if (vkQueueSubmit(vkState->graphicsQueue, 1, &submitInfo, vkState->inFlightFences[currentFrame]) != VK_SUCCESS) {
        DebugLog(L"failed to submit draw command buffer!");
        return false;
    }
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = {vkState->swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional
    vkQueuePresentKHR(vkState->presentQueue, &presentInfo);
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    return true;
}

inline void CleanupVulkan(VulkanState *vkState) {

    vkDeviceWaitIdle(vkState->device);

    vkDestroyBuffer(vkState->device, vkState->vertexBuffer, nullptr);
    vkDestroyBuffer(vkState->device, vkState->vertexBuffer2, nullptr);
    vkFreeMemory(vkState->device, vkState->vertexBufferMemory, nullptr);
    vkFreeMemory(vkState->device, vkState->vertexBufferMemory2, nullptr);
    for (int32 Frame = 0; Frame < MAX_FRAMES_IN_FLIGHT; Frame++) {
        vkDestroySemaphore(vkState->device, vkState->presentCompleteSemaphore[Frame], nullptr);
        vkDestroySemaphore(vkState->device, vkState->renderFinishedSemaphores[Frame], nullptr);
        vkDestroyFence(vkState->device, vkState->inFlightFences[Frame], nullptr);
    }
    vkDestroyCommandPool(vkState->device, vkState->commandPool, nullptr);

    vkDestroyPipeline(vkState->device, vkState->graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(vkState->device, vkState->pipelineLayout, nullptr);
    vkDestroyShaderModule(vkState->device, vkState->fragShaderModule, nullptr);
    vkDestroyShaderModule(vkState->device, vkState->vertShaderModule, nullptr);
    for (uint32 imageViewIndex = 0; imageViewIndex < vkState->swapchainImagesCount; imageViewIndex++) {
        vkDestroyImageView(vkState->device, vkState->swapChainImageViews[imageViewIndex], nullptr);
    }
    vkDestroySwapchainKHR(vkState->device, vkState->swapchain, nullptr);
    vkDestroySurfaceKHR(vkState->instance, vkState->surface, nullptr);
    vkDestroyDevice(vkState->device, nullptr);
    vkDestroyInstance(vkState->instance, nullptr);
}
