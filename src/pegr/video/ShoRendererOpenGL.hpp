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

#ifndef PEGR_SHORENDEREROGOPENGL_HPP
#define PEGR_SHORENDEREROGOPENGL_HPP

#ifdef PGG_OPENGL

#include <stdint.h>

#include "pegr/video/GraphicsApiLibrary.hpp"

#include "pegr/video/ShaderProgramResource.hpp"
#include "pegr/video/Scenegraph.hpp"
#include "pegr/video/Camera.hpp"

namespace pegr {

// sho = Spherical Harmonic per-Object renderer
class ShoRendererOg {
public:
    ShoRendererOg(uint32_t screenWidth, uint32_t screenHeight);
    ~ShoRendererOg();
    
    Camera mCamera;
    
private:
    Scenegraph* mScenegraph = nullptr;
    
    uint32_t mScreenWidth;
    uint32_t mScreenHeight;
    
    GLuint mTextureDepthStencil;
    GLuint mTextureShadowBias;
    GLuint mTextureSunlightIrradiance;
    GLuint mTextureForward;
    
    GLuint mFramebufferDepthPrepass;
    GLuint mFramebufferSunlightIrradiance;
    GLuint mFramebufferForward;

    uint32_t mSunlightShadowMapResolution;
    glm::vec3 mSunlightDirection;
    GLuint mSunlightFramebuffers[PGG_NUM_SUN_CASCADES];
    GLuint mSunlightDepthTextures[PGG_NUM_SUN_CASCADES];
    glm::mat4 mSunlightProjectionMatrices[PGG_NUM_SUN_CASCADES];
    glm::mat4 mSunlightViewMatrix;

    ShaderProgramResource* mSunlightIrradianceShaderProg;
    GLuint mPostProcessShaderHandle;
    
    ShaderProgramResource* mPostProcessShaderProg;
    GLuint mPostProcessShaderForwardHandle;
    
    GLuint mFullscreenVao;
    GLuint mFullscreenVbo;
    GLuint mFullscreenIbo;
public:
    
    bool initialize();
    bool cleanup();
    
    void renderFrame();
    
    void modelimapDepthPass(ModelInstance* modeli);
    void modelimapLightprobe(ModelInstance* modeli);
    void modelimapOpaque(ModelInstance* modeli);
    void modelimapTransparent(ModelInstance* modeli);
    
    void rebuildPipeline();
};
typedef ShoRendererOg ShoRenderer;

}

#endif // PGG_OPENGL

#endif // PEGR_SHORENDEREROGOPENGL_HPP
