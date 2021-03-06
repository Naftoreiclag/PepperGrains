/*
 *  Copyright 2016-2017 James Fong
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef PEGR_ENGINEVIDEO_HPP
#define PEGR_ENGINEVIDEO_HPP

#include <stdint.h>
#include <string>
#include <vector>

#include "pegr/video/GraphicsApiLibrary.hpp"
#include "pegr/misc/WindowInputSystemLibrary.hpp"

namespace pegr {
namespace Video {
    
    #ifdef PGG_OPENGL
    namespace OpenGL {
        // Initial is used to set value
        
        GLint getMajorVersion(GLint initial = 0);
        GLint getMinorVersion(GLint initial = 0);
        GLint getContextFlags(GLint initial = 0);
        
        GLint getMaxAtomicCounterBufferSize(GLint initial = 0); // Min 32
        GLint getMaxAtomicCounterBufferBindings(GLint initial = 0); // Min 1
        GLint getMaxShaderStorageBufferBindings(GLint initial = 0); // Min 8
        GLint getMaxCombinedShaderOutputResources(GLint initial = 0); // Min 8
        GLint getMaxCombinedShaderStorageBlocks(GLint initial = 0); // Min 8
        GLint getMaxUniformBlockSize(GLint initial = 0); // Min 16384
        //GLint getMaxTransformFeedbackBufferBindings(GLint initial = 0);
        GLint getMaxUniformBufferBindings(GLint initial = 0); // Min 84
        GLint getMaxDebugLoggedMessages(GLint initial = 0); // Min 1
        GLint getMaxDebugMessageLength(GLint initial = 0); // Min 1
        GLint getMaxColorAttachments(GLint initial = 0); // Min 8
        GLint getMaxColorTextureSamples(GLint initial = 0); // Min 1
        GLint getMaxDepthTextureSamples(GLint initial = 0); // Min 1
        GLint getMaxDrawBuffers(GLint initial = 0); // Min 8
        GLint getMaxFramebufferHeight(GLint initial = 0); // Min 16384
        GLint getMaxFramebufferWidth(GLint initial = 0); // Min 16384
        GLint getMaxFramebufferLayers(GLint initial = 0); // Min 2048
        GLint getMaxFramebufferSamples(GLint initial = 0); // Min 4
        
        bool isDebugOutputEnabled(bool initial = false);
        
        std::string getVersion(std::string initial = "");
        std::string getVendor(std::string initial = "");
        std::string getRenderer(std::string initial = "");
        std::string getShadingLanguageVersion(std::string initial = "");
        
        bool supportsGeometryShaders();
        bool supportsTessellationShaders();
        
        void queryOpenGL();
        
        bool initialize();
    }
    #endif // PGG_OPENGL
    
    #ifdef PGG_VULKAN
    namespace Vulkan {
        VkInstance getInstance();
        VkSurfaceKHR getSurface();
        VkPhysicalDevice getPhysicalDevice();
        VkDevice getLogicalDevice();
        
        VkQueue getComputeQueue();
        VkQueue getDisplayQueue();
        VkQueue getGraphicsQueue();
        VkQueue getSparseQueue();
        VkQueue getTransferQueue();
        
        int32_t getComputeQueueFamilyIndex();
        int32_t getDisplayQueueFamilyIndex();
        int32_t getGraphicsQueueFamilyIndex();
        int32_t getSparseQueueFamilyIndex();
        int32_t getTransferQueueFamilyIndex();
        
        VkCommandPool getComputeCommandPool();
        VkCommandPool getDisplayCommandPool();
        VkCommandPool getGraphicsCommandPool();
        VkCommandPool getSparseCommandPool();
        VkCommandPool getTransferCommandPool();
        
        VkSurfaceCapabilitiesKHR getSurfaceCapabilities();
        const std::vector<VkSurfaceFormatKHR> getAvailableSurfaceFormats();
        const std::vector<VkPresentModeKHR> getAvailablePresentModes();
        
        VkSwapchainKHR getSwapchain();
        VkFormat getSwapchainFormat();
        VkExtent2D getSwapchainExtent();
        const std::vector<VkImageView>& getSwapchainImageViews();
        const std::vector<VkImage>& getSwapchainImages();
        
        const std::vector<VkExtensionProperties>& getAvailableExtensions();
        const std::vector<VkLayerProperties>& getAvailableLayers();
        const std::vector<VkPhysicalDevice>& getAllPhysicalDevices();
        const std::vector<VkExtensionProperties>& getAvailablePhysicalDeviceExtensions();
        VkPhysicalDeviceProperties getPhysicalDeviceProperties();
        VkPhysicalDeviceFeatures getPhysicalDeviceFeatures();
        VkPhysicalDeviceMemoryProperties getPhysicalDeviceMemoryProperties();
        const std::vector<VkQueueFamilyProperties>& getQueueFamilies();
        
        bool initialize();
        bool cleanup();
    }
    #endif // PGG_VULKAN
    
    #ifdef PGG_SDL
    namespace SDL {
        std::string getName(std::string initial = "");
        bool isSoftwareFallback(bool initial = false);
        bool isHardwareAccelerated(bool initial = false);
        
        bool supportsTextureRender(bool initial = false);
        
        void querySDL(SDL_Renderer* renderer);
    }
    #endif // PGG_SDL
    
    #ifdef PGG_GFLW
    namespace GLFW {
    }
    #endif // PGG_GLFW
    
    uint32_t getWindowWidth();
    uint32_t getWindowHeight();
    float calcWindowAspectRatio();
    
    // In response to the window changing size; do not try to use for modifying the window
    void onWindowResize(uint32_t width, uint32_t height);
    
} // Video
} // pgg

#endif // PEGR_ENGINEVIDEO_HPP
