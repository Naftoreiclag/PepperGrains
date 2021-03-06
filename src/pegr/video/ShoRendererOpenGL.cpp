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

#ifdef PGG_OPENGL

#include <ShoRendererOgOpenGL.hpp>

#include "pegr/resource/Resources.hpp"
#include "pegr/misc/Logger.hpp"
#include "pegr/video/Video.hpp"

namespace pegr {

ShoRendererOg::ShoRendererOg()
: mScreenWidth(Video::getWindowWidth())
, mScreenHeight(Video::getWindowHeight())
, mScenegraph(nullptr)
, mCamera(Camera(glm::radians(45.f), 1.f, 0.1f, 10.f)) { }

bool ShoRendererOg::initialize() {
    Logger::log(Logger::VERBOSE) << "Constructing Sho Renderer..." << std::endl;
    
    // Generate texture render targets
    {
        // Depth-stencil mapping
        glGenTextures(1, &mTextureDepthStencil);
        glBindTexture(GL_TEXTURE_2D, mTextureDepthStencil);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, mScreenWidth, mScreenHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        // Shadow bias mapping
        glGenTextures(1, &mTextureShadowBias);
        glBindTexture(GL_TEXTURE_2D, mTextureShadowBias);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, mScreenWidth, mScreenHeight, 0, GL_RG, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        // Sunlight irradiance mapping
        glGenTextures(1, &mTextureSunlightIrradiance);
        glBindTexture(GL_TEXTURE_2D, mTextureSunlightIrradiance);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R, mScreenWidth, mScreenHeight, 0, GL_R, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        // Forward mapping
        glGenTextures(1, &mTextureForward);
        glBindTexture(GL_TEXTURE_2D, mTextureForward);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mScreenWidth, mScreenHeight, 0, GL_RGB, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    // Generate framebuffers
    {
        glGenFramebuffers(1, &mFramebufferDepthPrepass);
        glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferDepthPrepass);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureShadowBias, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mTextureDepthStencil, 0);
        
        glGenFramebuffers(1, &mFramebufferSunlightIrradiance);
        glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferSunlightIrradiance);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureSunlightIrradiance, 0);
        
        glGenFramebuffers(1, &mFramebufferForward);
        glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferForward);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureForward, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mTextureDepthStencil, 0);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    // GBuffer shader
    {
        mPostProcessShaderProg = ShaderProgramResource::gallop(Resources::find("sho.Postprocess.shaderProgram"));
        mPostProcessShaderProg->grab();
        const std::vector<ShaderProgramResource::Control>& sampler2DControls = mPostProcessShaderProg->getUniformSampler2Ds();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = sampler2DControls.begin(); iter != sampler2DControls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            
            if(entry.name == "forward") {
                mPostProcessShaderForwardHandle = entry.handle;
            }
        }
    }
    
    // Fullscreen quad
    {
        GLfloat vertices[] = {
            -1.f, -1.f,
             1.f, -1.f,
            -1.f,  1.f,
             1.f,  1.f
        };
        GLuint indices[] = {
            2, 0, 3,
            3, 0, 1,
        };
        
        glGenBuffers(1, &mFullscreenVbo);
        glBindBuffer(GL_ARRAY_BUFFER, mFullscreenVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &mFullscreenIbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFullscreenIbo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        glGenVertexArrays(1, &mFullscreenVao);
        glBindVertexArray(mFullscreenVao);

        glBindBuffer(GL_ARRAY_BUFFER, mFullscreenVbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mFullscreenIbo);

        glEnableVertexAttribArray(mPostProcessShaderProg->getPosAttrib());
        glVertexAttribPointer(mPostProcessShaderProg->getPosAttrib(), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));

        glBindVertexArray(0);
    }
    
    return true;
}

ShoRendererOg::~ShoRendererOg() { }

void ShoRendererOg::setScenegraph(Scenegraph* scenegraph) {
    mScenegraph = scenegraph;
}

bool ShoRendererOg::cleanup() {
    Logger::log(Logger::VERBOSE) << "Deleting Sho Renderer..." << std::endl;
    
    glDeleteTextures(1, &mTextureForward);
    glDeleteTextures(1, &mTextureDepthStencil);
    glDeleteTextures(1, &mTextureShadowBias);
    glDeleteTextures(1, &mTextureSunlightIrradiance);
    glDeleteFramebuffers(1, &mFramebufferDepthPrepass);
    glDeleteFramebuffers(1, &mFramebufferForward);
    glDeleteFramebuffers(1, &mFramebufferSunlightIrradiance);
    
    return true;
}

void ShoRendererOg::renderFrame() {
    if(!mScenegraph) {
        return;
    }
    
    glViewport(0, 0, mScreenWidth, mScreenHeight);
    
    // Depth prepass / Sunlight bias material information
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFramebufferDepthPrepass);
        GLuint colorAttachments[] = {
            GL_COLOR_ATTACHMENT0
        };
        glDrawBuffers(1, colorAttachments);
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDisable(GL_STENCIL_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glDisable(GL_BLEND);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        mScenegraph->processAll(std::bind(&ShoRendererOg::modelimapDepthPass, this, std::placeholders::_1));
    }
    
    // Cascaded shadow map generation
    {
        
    }
    
    // Sunlight irradiance calculation
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFramebufferSunlightIrradiance);
        GLuint colorAttachments[] = {
            GL_COLOR_ATTACHMENT0
        };
        glDrawBuffers(1, colorAttachments);
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        glDepthFunc(GL_EQUAL);
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        // Temporary
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        /*
        glUseProgram(mSunlightIrradianceShaderProg->getHandle());
    
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, mTextureForward);
        glUniform1i(mPostProcessShaderForwardHandle, 0);
        
        glBindVertexArray(mFullscreenVao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        glUseProgram(0);
        */
    }
    
    // Geometry pass
    {
        // Populate per-object lightprobe data
        mScenegraph->processAll(std::bind(&ShoRendererOg::modelimapLightprobe, this, std::placeholders::_1));
        
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFramebufferForward);
        GLuint colorAttachments[] = {
            GL_COLOR_ATTACHMENT0
        };
        glDrawBuffers(1, colorAttachments);
        glClearColor(0.f, 1.f, 1.f, 1.f);
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDisable(GL_STENCIL_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glDisable(GL_BLEND);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Opaque geometry
        mScenegraph->processAll(std::bind(&ShoRendererOg::modelimapOpaque, this, std::placeholders::_1));
        
        // Transparent
        mScenegraph->processAll(std::bind(&ShoRendererOg::modelimapTransparent, this, std::placeholders::_1));
    }
    
    // Skybox
    {
        
    }
    
    // Post-process and render to screen
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        glDepthFunc(GL_EQUAL);
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        glUseProgram(mPostProcessShaderProg->getHandle());
    
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, mTextureForward);
        glUniform1i(mPostProcessShaderForwardHandle, 0);
        
        glBindVertexArray(mFullscreenVao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        glUseProgram(0);
    }
    
    // TODO: disable double buffering; we already have our own "other" buffer
}

void ShoRendererOg::modelimapDepthPass(ModelInstance* modeli) {
}

void ShoRendererOg::modelimapLightprobe(ModelInstance* modeli) {
    const std::vector<Geometry::Lightprobe>& lightprobes = modeli->getModel()->getGeometry()->getLightprobes();
    
    std::vector<Geometry::Lightprobe>::const_iterator liter = lightprobes.begin();
    std::vector<Spharm>::iterator siter = modeli->mLightprobeData.begin();
    
    // TODO: remove one of these
    while(liter != lightprobes.end() || siter != modeli->mLightprobeData.end()) {
        Spharm& spharm = *siter;
        const Geometry::Lightprobe& lprobe = *liter;
        
        // ...
    }
}

void ShoRendererOg::modelimapOpaque(ModelInstance* modeli) {
    Renderable::Pass rendPass(Renderable::Pass::Type::SHO_FORWARD);
    rendPass.mScreenWidth = mScreenWidth;
    rendPass.mScreenHeight = mScreenHeight;
    rendPass.mCamera = mCamera;
    
    Model* model = modeli->getModel();
    Material* material = model->getMaterial();
    Geometry* geometry = model->getGeometry();
    /*
    if(!material->isVisible(rendPass)) {
        return;
    }
    
    material->useProgram(rendPass, modeli->mModelMatr);

    // Bind the vertex array object from earlier (i.e. vertex attribute and geometry buffer info)
    model->bindVertexArray();

    // Draw elements as specified by the geometry
    geometry->drawElements();
     */
}
void ShoRendererOg::modelimapTransparent(ModelInstance* modeli) {
    
}
void ShoRendererOg::rebuildPipeline() {
    
}

}

#endif // PGG_OPENGL
