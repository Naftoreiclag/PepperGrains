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

#include "pegr/deprecated/ForwardRenderer.hpp"

#include "pegr/resource/Resources.hpp"

namespace pegr {

ForwardRenderer::ForwardRenderer(uint32_t width, uint32_t height, Renderable* renderable)
: mScreenWidth(width)
, mScreenHeight(height)
, mRenderable(renderable) {
}

ForwardRenderer::~ForwardRenderer() {
    
}

void ForwardRenderer::load() {
    
    // Create renderbuffer/textures for deferred shading
    {
        // Forward mapping
        glGenTextures(1, &mGBuff.forwardTexture);
        glBindTexture(GL_TEXTURE_2D, mGBuff.forwardTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mScreenWidth, mScreenHeight, 0, GL_RGB, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        // Depth-stencil mapping
        glGenTextures(1, &mGBuff.depthStencilTexture);
        glBindTexture(GL_TEXTURE_2D, mGBuff.depthStencilTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, mScreenWidth, mScreenHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    // Create framebuffers
    {
        glGenFramebuffers(1, &mGBuff.framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, mGBuff.framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mGBuff.forwardTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mGBuff.depthStencilTexture, 0);
        
        GLuint colorAttachments[] = {
            GL_COLOR_ATTACHMENT0
        };
        glDrawBuffers(1, colorAttachments);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    // GBuffer shader
    {
        mScreenShader.shaderProg = ShaderProgramResource::upcast(Resources::find("forward.Tonemapper.shaderProgram"));
        mScreenShader.shaderProg->grab();
        const std::vector<ShaderProgramResource::Control>& sampler2DControls = mScreenShader.shaderProg->getUniformSampler2Ds();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = sampler2DControls.begin(); iter != sampler2DControls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            
            if(entry.name == "forward") {
                mScreenShader.forwardHandle = entry.handle;
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

        glEnableVertexAttribArray(mScreenShader.shaderProg->getPosAttrib());
        glVertexAttribPointer(mScreenShader.shaderProg->getPosAttrib(), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));

        glBindVertexArray(0);
    }
}

void ForwardRenderer::unload() {
    glDeleteBuffers(1, &mFullscreenIbo);
    glDeleteBuffers(1, &mFullscreenVbo);
    glDeleteVertexArrays(1, &mFullscreenVao);
    
    glDeleteTextures(1, &mGBuff.forwardTexture);
    glDeleteTextures(1, &mGBuff.depthStencilTexture);
    glDeleteFramebuffers(1, &mGBuff.framebuffer);
    
    mScreenShader.shaderProg->drop();
}

void ForwardRenderer::renderFrame() {
    if(!mRenderable) return;
    
    {
        glViewport(0, 0, mScreenWidth, mScreenHeight);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBuff.framebuffer);
        GLuint colorAttachments[] = {
            GL_COLOR_ATTACHMENT0
        };
        glDrawBuffers(1, colorAttachments);
        glClearColor(1.f, 1.f, 0.f, 1.f);
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDisable(GL_STENCIL_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glDisable(GL_BLEND);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        Renderable::Pass geometryRenderPass(Renderable::Pass::Type::GEOMETRY);
        /*
        geometryRenderPass.viewMat = mCamera.viewMat;
        geometryRenderPass.projMat = mCamera.projMat;
        geometryRenderPass.camPos = mCamera.position;
        geometryRenderPass.camDir = glm::vec3(glm::inverse(mCamera.viewMat) * glm::vec4(0.0, 0.0, -1.0, 0.0));
        geometryRenderPass.nearPlane = mCamera.nearDepth;
        geometryRenderPass.farPlane = mCamera.farDepth;
        geometryRenderPass.setScreenSize(mScreenWidth, mScreenHeight);
        */
        
        mRenderable->render(geometryRenderPass);
    }
    
    // Perform post-process
    {
        
    }
    
    // Render to screen
    {
        glViewport(0, 0, mScreenWidth, mScreenHeight);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        glDepthFunc(GL_EQUAL);
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        glUseProgram(mScreenShader.shaderProg->getHandle());
    
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, mGBuff.forwardTexture);
        glUniform1i(mScreenShader.forwardHandle, 0);
        
        glBindVertexArray(mFullscreenVao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        glUseProgram(0);
    }
    
    // TODO: disable double buffering; we already have our own "other" buffer
}


}

