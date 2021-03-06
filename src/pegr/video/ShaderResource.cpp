/*
 *  Copyright 2015-2017 James Fong
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

#include "pegr/video/ShaderResource.hpp"

#include <cassert>
#include <fstream>
#include <sstream>
#include <string>

#include "pegr/misc/Logger.hpp"
#include "pegr/misc/StreamStuff.hpp"
#include "pegr/video/Video.hpp"

namespace pegr {

ShaderResource::ShaderResource(ShaderResource::Type type)
: mType(type)
, mLoaded(false)
, Resource(Resource::Type::SHADER) { }

ShaderResource::~ShaderResource() { }

ShaderResource* ShaderResource::gallop(Resource* resource) {
    if(!resource || resource->mResourceType != Resource::Type::SHADER) {
        Logger::log(Logger::WARN) << "Failed to cast " << (resource ? resource->getName() : "nullptr") << " to shader!" << std::endl;
        return nullptr;
    } else {
        return static_cast<ShaderResource*>(resource);
    }
}

#ifdef PGG_VULKAN
VkShaderStageFlagBits ShaderResource::toVulkanShaderStageFlagBits(Type t) {
    switch(t) {
        case Type::COMPUTE: return VK_SHADER_STAGE_COMPUTE_BIT;
        case Type::VERTEX: return VK_SHADER_STAGE_VERTEX_BIT;
        case Type::TESS_CONTROL: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case Type::TESS_EVALUATION: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        case Type::GEOMETRY: return VK_SHADER_STAGE_GEOMETRY_BIT;
        case Type::FRAGMENT: return VK_SHADER_STAGE_FRAGMENT_BIT;
        default: return VK_SHADER_STAGE_ALL; // ??? There is no empty flag
    }
}
VkShaderStageFlagBits ShaderResource::getShaderStageFlagBits() {
    return toVulkanShaderStageFlagBits(mType);
}
void ShaderResource::load() {
    assert(!mLoaded && "Attempted to load shader that is already loaded");
    std::vector<uint8_t> bytecode;
    if(!readFileToByteBuffer(this->getFile().string(), bytecode)) {
        Logger::log(Logger::WARN) << "Could not read shader" << std::endl;
    }
    VkShaderModuleCreateInfo cstrArgs; {
        cstrArgs.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        cstrArgs.pNext = nullptr;
        cstrArgs.flags = 0;
        cstrArgs.codeSize = bytecode.size();
        cstrArgs.pCode = reinterpret_cast<const uint32_t*>(bytecode.data());
    }
    if(vkCreateShaderModule(Video::Vulkan::getLogicalDevice(), &cstrArgs, nullptr, &mHandle) != VK_SUCCESS) {
        Logger::log(Logger::WARN) << "Could not create shader module" << std::endl;
    }
    mPipelineShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    mPipelineShaderStageInfo.pNext = nullptr;
    mPipelineShaderStageInfo.flags = 0;
    mPipelineShaderStageInfo.stage = toVulkanShaderStageFlagBits(mType);
    mPipelineShaderStageInfo.module = mHandle;
    mPipelineShaderStageInfo.pName = "main";
    mPipelineShaderStageInfo.pSpecializationInfo = nullptr;
    mLoaded = true;
}
void ShaderResource::unload() {
    assert(mLoaded && "Attempted to unload shader before loading it");
    vkDestroyShaderModule(Video::Vulkan::getLogicalDevice(), mHandle, nullptr);
    mPipelineShaderStageInfo = VkPipelineShaderStageCreateInfo();
    mLoaded = false;
}
VkShaderModule ShaderResource::getHandle() {
    return mHandle;
}
VkPipelineShaderStageCreateInfo ShaderResource::getPipelineShaderStageInfo() {
    return mPipelineShaderStageInfo;
}
#endif // PGG_VULKAN

#ifdef PGG_OPENGL
void ShaderResource::load() {
    assert(!mLoaded && "Attempted to load shader that is already loaded");

    std::ifstream loader(this->getFile().string().c_str());
    std::stringstream ss;
    ss << loader.rdbuf();
    loader.close();

    std::string shaderSrcStr = ss.str();
    const GLchar* shaderSrc = shaderSrcStr.c_str();

    #ifdef PGG_OPENGL
    switch(mType) {
        case COMPUTE: {
            mHandle = glCreateShader(GL_COMPUTE_SHADER);
            break;
        }
        case VERTEX: {
            mHandle = glCreateShader(GL_VERTEX_SHADER);
            break;
        }
        case TESS_CONTROL: {
            mHandle = glCreateShader(GL_TESS_CONTROL_SHADER);
            break;
        }
        case TESS_EVALUATION: {
            mHandle = glCreateShader(GL_TESS_EVALUATION_SHADER);
            break;
        }
        case GEOMETRY: {
            mHandle = glCreateShader(GL_GEOMETRY_SHADER);
            break;
        }
        case FRAGMENT: {
            mHandle = glCreateShader(GL_FRAGMENT_SHADER);
            break;
        }
        default: {
            assert(false && "Unknown shader type?");
            break; // what
        }
    }
    glShaderSource(mHandle, 1, &shaderSrc, 0);
    glCompileShader(mHandle);
    
    GLint compileStatus;
    glGetShaderiv(mHandle, GL_COMPILE_STATUS, &compileStatus);
    if(compileStatus == GL_FALSE) {
        Logger::Out wlog = Logger::log(Logger::WARN);
        wlog << "Error while compiling ";
        switch(mType) {
            case COMPUTE: {
                wlog << "COMPUTE";
                break;
            }
            case VERTEX: {
                wlog << "VERTEX";
                break;
            }
            case TESS_CONTROL: {
                wlog << "TESS CONTROL";
                break;
            }
            case TESS_EVALUATION: {
                wlog << "TESS EVALUATION";
                break;
            }
            case GEOMETRY: {
                wlog << "GEOMETRY";
                break;
            }
            case FRAGMENT: {
                wlog << "FRAGMENT";
                break;
            }
            default: {
                break; // what
            }
        }
        wlog << " shader " << this->getName() << std::endl;
        char infoLog[512];
        glGetShaderInfoLog(mHandle, 512, 0, infoLog);
        wlog << infoLog;
        wlog << shaderSrcStr;
        wlog << std::endl;
    } else {
        Logger::log(Logger::VERBOSE) << "Compiled shader " << this->getName() << std::endl;
    }
    
    #endif

    mLoaded = true;
}
void ShaderResource::unload() {
    assert(mLoaded && "Attempted to unload shader before loading it");
    glDeleteShader(mHandle);
    mLoaded = false;
}
GLuint ShaderResource::getHandle() {
    return mHandle;
}
#endif // PGG_OPENGL

}
