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

#include "pegr/video/Geometry.hpp"

#include "pegr/video/GeometryResource.hpp"
#include "pegr/misc/Logger.hpp"

namespace pegr {

Geometry* Geometry::getFallback() {
    static Geometry geometry;
    return &geometry;
}
bool Geometry::hasLightprobes() const { return false; }
const std::vector<Geometry::Lightprobe>& Geometry::getLightprobes() const {
    static std::vector<Lightprobe> dummy;
    return dummy;
}
bool Geometry::hasArmature() const { return false; }
const Geometry::Armature& Geometry::getArmature() const {
    static Armature dummy;
    return dummy;
}

#ifdef PGG_OPENGL
void Geometry::enablePositionAttrib(GLuint posAttrib) { }
void Geometry::enableColorAttrib(GLuint colorAttrib) { }
void Geometry::enableUVAttrib(GLuint textureAttrib) { }
void Geometry::enableNormalAttrib(GLuint normalAttrib) { }
void Geometry::enableTangentAttrib(GLuint tangentAttrib) { }
void Geometry::enableBitangentAttrib(GLuint bitangentAttrib) { }
void Geometry::enableBoneAttrib(GLuint boneWeightAttrib, GLuint boneIndexAttrib) { }
#endif

#ifdef PGG_VULKAN
const VkPipelineVertexInputStateCreateInfo* Geometry::getVertexInputState() { return nullptr; }
const VkPipelineInputAssemblyStateCreateInfo* Geometry::getInputAssemblyState() { return nullptr; }
void Geometry::cmdBindBuffers(VkCommandBuffer cmdBuff) { }
void Geometry::cmdDrawIndexed(VkCommandBuffer cmdBuff) { }
#endif

void Geometry::load() { }
void Geometry::unload() { }
}

