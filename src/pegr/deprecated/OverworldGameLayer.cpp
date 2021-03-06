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

#include "pegr/deprecated/OverworldGameLayer.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>

#include <glm/gtx/string_cast.hpp>
#include <SDL2/SDL.h>

#include "pegr/deprecated/InputMoveESignal.hpp"
#include "pegr/deprecated/DebugFPControllerEListe.hpp"
#include "pegr/deprecated/SceneNodeEComp.hpp"
#include "pegr/deprecated/SceneNodeESys.hpp"
#include "pegr/deprecated/SunLightModel.hpp"
#include "pegr/deprecated/DirectionalLightModel.hpp"
#include "pegr/deprecated/PointLightModel.hpp"
#include "pegr/deprecated/RigidBodyEComp.hpp"
#include "pegr/deprecated/GrassModel.hpp"
#include "pegr/deprecated/TessModel.hpp"
#include "pegr/math/Vec3.hpp"

namespace pegr {

OverworldGameLayer::OverworldGameLayer(uint32_t width, uint32_t height)
: mScreenWidth(width)
, mScreenHeight(height) {
}

OverworldGameLayer::~OverworldGameLayer() {
}

// Lifecycle
void OverworldGameLayer::onBegin() {
    SDL_SetRelativeMouseMode(SDL_TRUE);
    
    ResourceManager* resman = ResourceManager::getSingleton();
    
    loadGBuffer();
    loadSun();

    mEntityWorld = new nres::World();

    mBroadphase = new btDbvtBroadphase();
    mCollisionConfiguration = new btDefaultCollisionConfiguration();
    mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
    mSolver = new btSequentialImpulseConstraintSolver();
    mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfiguration);
    mDynamicsWorld->setGravity(btVector3(0, -9.8, 0));
    mRigidBodyESys = new RigidBodyESys(mDynamicsWorld);
    mEntityWorld->attachSystem(mRigidBodyESys);
    
    btStaticPlaneShape* planeShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
    mPlaneRigid = new btRigidBody(0, 0, planeShape);
    mDynamicsWorld->addRigidBody(mPlaneRigid);
    
    mRootNode = new SceneNode();
    mRootNode->grab();
    mSceneNodeESys = new SceneNodeESys(mRootNode);
    mEntityWorld->attachSystem(mSceneNodeESys);
    
    mPlayerEntity = mEntityWorld->newEntity();
    mPlayerEntity->add(new SceneNodeEComp());
    mPlayerEntity->addListener(new DebugFPControllerEListe());
    mPlayerEntity->publish();
    
    nres::Entity* cube;
    cube = mEntityWorld->newEntity();
    cube->add(new SceneNodeEComp(resman->findModel("RoseCube.model")));
    cube->add(new RigidBodyEComp(new btBoxShape(Vec3(1.f, 1.f, 1.f)), Vec3(-3.f, 3.f, -3.f)));
    cube->publish();
    
    cube = mEntityWorld->newEntity();
    cube->add(new SceneNodeEComp(resman->findModel("RoseCube.model")));
    cube->add(new RigidBodyEComp(new btBoxShape(Vec3(1.f, 1.f, 1.f)), Vec3(-4.5f, 8.f, -4.5f)));
    cube->publish();
    
    for(uint8_t i = 0; i < 24; ++ i) {
        mTestCubes[i] = mRootNode->newChild()->grabModel(resman->findModel("RoseCube.model"));
        mTestCubes[i]->grab();
        mTestCubes[i]->setLocalScale(glm::vec3(0.2f));
        mTestCubes[i]->setLocalTranslation(glm::vec3(-2000.f, -2000.f, -2000.f));
    }

    mComputer = resman->findShaderProgram("ComputeTest.shaderProgram");
    mComputer->grab();
    
    mRoseTexture = resman->findTexture("128Rose.texture");
    mRoseTexture->grab();
    
    /*
    glUseProgram(mComputer->getHandle());
    
    glActiveTexture(GL_TEXTURE0 + 0);
    //glBindImageTexture(0, mRoseTexture->getHandle(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    
    glDispatchCompute(mScreenWidth / 8, mScreenHeight / 8, 1);
    glUseProgram(0);
    */
    mComputer->drop();
    
    //mRootNode->newChild()->grabModel(resman->findModel("TestPlane.model"));
    mRootNode->newChild()->grabModel(new GrassModel());
    //mRootNode->newChild()->attachModel(resman->findModel("Door.model"));
    //mRootNode->newChild()->grabModel(resman->findModel("RoseCube.model"));
    mRootNode->newChild()->move(glm::vec3(1.5f, 1.5f, 1.5f))->grabModel(new PointLightModel(glm::vec3(2.f, 0.f, 2.f), 2.f));
    mRootNode->newChild()->move(glm::vec3(-3.f, 3.f, -3.f))->grabModel(new TessModel());
    mRootNode->newChild()->move(glm::vec3(-3.f, 3.f, -3.f))->grabModel(resman->findModel("Door.model"));
    
    
    rainstormFont = resman->findFont("Rainstorm.font");
    rainstormFont->grab();

    fpsCounter = new TextModel(rainstormFont, "FPS: Calculating...");
    fpsCounter->grab();
    
    mCamLocNode = new SceneNode();
    mCamLocNode->move(Vec3(0.f, 1.5f, 0.f));
    
    mCamRollNode = new SceneNode();
    mCamPitchNode = new SceneNode();
    mCamYawNode = new SceneNode();
    mCamLocNode->addChild(mCamYawNode);
    mCamYawNode->addChild(mCamPitchNode);
    mCamPitchNode->addChild(mCamRollNode);
    
    mCamera.fov = glm::radians(90.f);
    mCamera.aspect = ((float) mScreenWidth) / ((float) mScreenHeight);
    mCamera.farDepth = 400.f;
    mCamera.nearDepth = 0.1f;
    // Find cascade borders
    mCamera.cascadeBorders[0] = mCamera.nearDepth;
    mCamera.cascadeBorders[PGG_NUM_SUN_CASCADES] = mCamera.farDepth;
    for(uint8_t i = 1; i < PGG_NUM_SUN_CASCADES; ++ i) {
        // TODO: replace + 1.f with something more meaningful
        float near = mCamera.nearDepth + 1.f;
        float far = mCamera.farDepth;
        
        mCamera.cascadeBorders[i] = near * std::pow(far / near, ((float) i) / ((float) PGG_NUM_SUN_CASCADES));
    }
    
    for(uint8_t i = 0; i < PGG_NUM_SUN_CASCADES + 1; ++ i) {
        std::cout << "Slice " << ((uint32_t) i) << " " << mCamera.cascadeBorders[i] << std::endl;
    }
    
    SceneNodeEComp* plS = (SceneNodeEComp*) mPlayerEntity->getComponent(SceneNodeEComp::sComponentID);
    plS->mSceneNode->addChild(mCamLocNode);
    
    //mTerrainModel->grab();
    
    mInfCheck = new InfiniteCheckerboardModel();
    mTerrainModel = new TerrainModel();
    mInfCheck->grab();
    mRootNode->newChild()->grabModel(mInfCheck);
    //mRootNode->newChild()->grabModel(mTerrainModel);
    

    fps = 0.f;
    fpsWeight = 0.85f;
    
    mDebugWireframe = false;

    oneSecondTimer = 0.f;
}
void OverworldGameLayer::onEnd() {
    unloadGBuffer();
    unloadSun();
    
    for(uint8_t i = 0; i < 24; ++ i) {
        mTestCubes[i]->drop();
    }
    
    mInfCheck->drop();
    mRootNode->drop();
    
    //mTerrainModel->drop();
    
    fpsCounter->drop();
    rainstormFont->drop();
    delete mSceneNodeESys;
    delete mRigidBodyESys;
    delete mDynamicsWorld;
    delete mSolver;
    delete mDispatcher;
    delete mCollisionConfiguration;
    delete mBroadphase;
}

void OverworldGameLayer::onTick(float tpf, const Uint8* keyStates) {
    
    glm::vec3 movement;
    if(keyStates[SDL_GetScancodeFromKey(SDLK_w)]) {
        movement.z -= 1.0;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_a)]) {
        movement.x -= 1.0;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_s)]) {
        movement.z += 1.0;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_d)]) {
        movement.x += 1.0;
    }
    if(movement != glm::vec3(0.f)) {
        glm::normalize(movement);
        if(keyStates[SDL_GetScancodeFromKey(SDLK_LSHIFT)]) {
            movement *= 10.f;
        }
        
        movement = glm::vec3(mCamRollNode->calcWorldTransform() * glm::vec4(movement, 0.f) * tpf);
        
        mPlayerEntity->broadcast(new InputMoveESignal(movement));
        //mCamLocNode->move(movement);
    }

    SceneNodeEComp* comp = (SceneNodeEComp*) mPlayerEntity->getComponent(SceneNodeEComp::sComponentID);
    mInfCheck->setFocus(comp->mSceneNode->getLocalTranslation());
    
    glm::vec4 debugShow;
    if(keyStates[SDL_GetScancodeFromKey(SDLK_1)]) {
        debugShow.x = 1.f;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_2)]) {
        debugShow.y = 1.f;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_3)]) {
        debugShow.z = 1.f;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_4)]) {
        debugShow.w = 1.f;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_5)]) {
        mDebugWireframe = true;
    }
    if(keyStates[SDL_GetScancodeFromKey(SDLK_6)]) {
        mDebugWireframe = false;
    }
    
    if(keyStates[SDL_GetScancodeFromKey(SDLK_q)]) {
        mSky.sunDirection = glm::vec3(mCamRollNode->calcWorldTransform() * glm::vec4(0.f, 0.f, -1.f, 0.f));
    }
    
    if(keyStates[SDL_GetScancodeFromKey(SDLK_r)]) {
        ResourceManager* resman = ResourceManager::getSingleton();
        
        nres::Entity* cube;
        cube = mEntityWorld->newEntity();
        cube->add(new SceneNodeEComp(resman->findModel("RoseCube.model")));
        cube->add(new RigidBodyEComp(new btBoxShape(Vec3(1.f, 1.f, 1.f)), Vec3(0.f, 3.f, 0.f) + mCamLocNode->calcWorldTranslation()));
        cube->publish();
    }
    mDynamicsWorld->stepSimulation(tpf, 5);
    mRigidBodyESys->onTick();
    mSceneNodeESys->onTick(tpf);
    
    mCamera.viewMat = glm::inverse(mCamRollNode->calcWorldTransform());
    mCamera.projMat = glm::perspective(mCamera.fov, mCamera.aspect, mCamera.nearDepth, mCamera.farDepth);
    mCamRollNode->calcWorldTranslation(mCamera.position);
    
    
    renderFrame(debugShow, mDebugWireframe);
    
    if(tpf > 0) {
        float fpsNew = 1 / tpf;
        fps = (fps * fpsWeight) + (fpsNew * (1.f - fpsWeight));
    }

    oneSecondTimer += tpf;
    if(oneSecondTimer > 1.f) {
        oneSecondTimer -= 1.f;

        fpsCounter->drop();

        std::stringstream ss;
        ss << "FPS: ";
        ss << (uint32_t) fps;

        fpsCounter = new TextModel(rainstormFont, ss.str());
        fpsCounter->grab();
        
        std::cout << "Cam location: " << glm::to_string(mCamLocNode->calcWorldTransform()[3]) << std::endl;
    }
    
    glm::mat4 viewMatOverlay;
    glm::mat4 projMatOverlay = glm::ortho(0.f, (float) mScreenWidth, 0.f, (float) mScreenHeight);
    
    Model::RenderPass fpsRPC(Model::RenderPassType::SCREEN);
    fpsRPC.viewMat = viewMatOverlay;
    fpsRPC.projMat = projMatOverlay;
    fpsCounter->render(fpsRPC, glm::mat4());
}

void OverworldGameLayer::loadGBuffer() {
    // Create renderbuffer/textures for deferred shading
    {
        // Diffuse mapping
        glGenTextures(1, &mGBuff.diffuseTexture);
        glBindTexture(GL_TEXTURE_2D, mGBuff.diffuseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mScreenWidth, mScreenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // Normal mapping
        glGenTextures(1, &mGBuff.normalTexture);
        glBindTexture(GL_TEXTURE_2D, mGBuff.normalTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mScreenWidth, mScreenHeight, 0, GL_RGB, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // Bright mapping
        glGenTextures(1, &mGBuff.brightTexture);
        glBindTexture(GL_TEXTURE_2D, mGBuff.brightTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, mScreenWidth, mScreenHeight, 0, GL_RGB, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        // DepthStencil mapping
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
        glGenFramebuffers(1, &mGBuff.gFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, mGBuff.gFramebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mGBuff.diffuseTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mGBuff.normalTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mGBuff.brightTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, mGBuff.depthStencilTexture, 0);
        
        GLuint colorAttachments[] = {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1
        };
        glDrawBuffers(2, colorAttachments);
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "G Complete" << std::endl;
        }
        else {
            std::cout << "G Incomplete" << std::endl;
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    ResourceManager* resman = ResourceManager::getSingleton();
    // GBuffer shader
    {
        mScreenShader.shaderProg = resman->findShaderProgram("GBuffer.shaderProgram");
        mScreenShader.shaderProg->grab();
        const std::vector<ShaderProgramResource::Control>& sampler2DControls = mScreenShader.shaderProg->getUniformSampler2Ds();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = sampler2DControls.begin(); iter != sampler2DControls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            
            if(entry.name == "diffuse") {
                mScreenShader.diffuseHandle = entry.handle;
            }
            else if(entry.name == "bright") {
                mScreenShader.brightHandle = entry.handle;
            }
        }
    }
    // Debug shader
    {
        mDebugScreenShader.shaderProg = resman->findShaderProgram("GBufferDebug.shaderProgram");
        mDebugScreenShader.shaderProg->grab();
        const std::vector<ShaderProgramResource::Control>& sampler2DControls = mDebugScreenShader.shaderProg->getUniformSampler2Ds();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = sampler2DControls.begin(); iter != sampler2DControls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            
            if(entry.name == "diffuse") {
                mDebugScreenShader.diffuseHandle = entry.handle;
            }
            else if(entry.name == "normal") {
                mDebugScreenShader.normalHandle = entry.handle;
            }
            else if(entry.name == "depth") {
                mDebugScreenShader.depthHandle = entry.handle;
            }
            else if(entry.name == "bright") {
                mDebugScreenShader.brightHandle = entry.handle;
            }
        }
        const std::vector<ShaderProgramResource::Control>& vec4Controls = mDebugScreenShader.shaderProg->getUniformVec4s();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = vec4Controls.begin(); iter != vec4Controls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            
            if(entry.name == "showWhat") {
                mDebugScreenShader.viewHandle = entry.handle;
            }
        }
        
        assert(mDebugScreenShader.shaderProg->needsInvViewProjMatrix() && "Debug G-buffer shader does not accept inverse view projection matrix");
    }
    // Debug sun shader
    {
        mDebugSunShader.shaderProg = resman->findShaderProgram("SunDepthDebug.shaderProgram");
        mDebugSunShader.shaderProg->grab();
        const std::vector<ShaderProgramResource::Control>& sampler2DControls = mDebugSunShader.shaderProg->getUniformSampler2Ds();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = sampler2DControls.begin(); iter != sampler2DControls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            
            if(entry.name == "map0") {
                mDebugSunShader.mapHandles[0] = entry.handle;
            }
            else if(entry.name == "map1") {
                mDebugSunShader.mapHandles[1] = entry.handle;
            }
            else if(entry.name == "map2") {
                mDebugSunShader.mapHandles[2] = entry.handle;
            }
            else if(entry.name == "map3") {
                mDebugSunShader.mapHandles[3] = entry.handle;
            }
        }
        const std::vector<ShaderProgramResource::Control>& vec4Controls = mDebugSunShader.shaderProg->getUniformVec4s();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = vec4Controls.begin(); iter != vec4Controls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            
            if(entry.name == "showWhat") {
                mDebugSunShader.viewHandle = entry.handle;
            }
        }
    }
    // Sky stencil shader
    {
        mSkyStencilShader.shaderProg = resman->findShaderProgram("SkyStencil.shaderProgram");
        mSkyStencilShader.shaderProg->grab();
    }
    // Fill screen shader
    {
        mFillScreenShader.shaderProg = resman->findShaderProgram("FillScreen.shaderProgram");
        mFillScreenShader.shaderProg->grab();
        const std::vector<ShaderProgramResource::Control>& vec3Controls = mFillScreenShader.shaderProg->getUniformVec3s();
        for(std::vector<ShaderProgramResource::Control>::const_iterator iter = vec3Controls.begin(); iter != vec3Controls.end(); ++ iter) {
            const ShaderProgramResource::Control& entry = *iter;
            
            if(entry.name == "color") {
                mFillScreenShader.colorHandle = entry.handle;
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
        
        glEnableVertexAttribArray(mDebugScreenShader.shaderProg->getPosAttrib());
        glVertexAttribPointer(mDebugScreenShader.shaderProg->getPosAttrib(), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));
        
        glEnableVertexAttribArray(mSkyStencilShader.shaderProg->getPosAttrib());
        glVertexAttribPointer(mSkyStencilShader.shaderProg->getPosAttrib(), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));

        glBindVertexArray(0);
    }
}
void OverworldGameLayer::unloadGBuffer() {
    glDeleteBuffers(1, &mFullscreenIbo);
    glDeleteBuffers(1, &mFullscreenVbo);
    glDeleteVertexArrays(1, &mFullscreenVao);
    
    glDeleteTextures(1, &mGBuff.diffuseTexture);
    glDeleteTextures(1, &mGBuff.normalTexture);
    glDeleteTextures(1, &mGBuff.depthStencilTexture);
    glDeleteTextures(1, &mGBuff.brightTexture);
    glDeleteFramebuffers(1, &mGBuff.gFramebuffer);

    mScreenShader.shaderProg->drop();
    mDebugScreenShader.shaderProg->drop();
    mSkyStencilShader.shaderProg->drop();
    mFillScreenShader.shaderProg->drop();
}

void OverworldGameLayer::loadSun() {
    mSky.sunTextureSize = 1024;
    mSky.sunDirection = glm::normalize(glm::vec3(-1.f, -1.f, -1.f));
    mSky.sunPosition = glm::vec3(1.f, 1.f, 1.f);
    mSky.sunModel = new SunLightModel(glm::vec3(1.0f, 1.0f, 1.0f));
    mSky.sunModel->grab();
    
    // Depth mapping
    glGenTextures(PGG_NUM_SUN_CASCADES, mSky.sunDepthTextures);
    
    for(uint8_t i = 0; i < PGG_NUM_SUN_CASCADES; ++ i) {
        glBindTexture(GL_TEXTURE_2D, mSky.sunDepthTextures[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, mSky.sunTextureSize, mSky.sunTextureSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glGenFramebuffers(PGG_NUM_SUN_CASCADES, mSky.sunFramebuffers);
    for(uint8_t i = 0; i < PGG_NUM_SUN_CASCADES; ++ i) {
        glBindFramebuffer(GL_FRAMEBUFFER, mSky.sunFramebuffers[i]);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, mSky.sunDepthTextures[i], 0);
        
        // What do these do?
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Sun " << i << " Complete" << std::endl;
        }
        else {
            std::cout << "Sun " << i << " Incomplete" << std::endl;
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    for(uint8_t i = 0; i < 8; ++ i) {
                glm::vec4 corner(
                    (i & (1 << 0)) ? -1.f : 1.f,
                    (i & (1 << 1)) ? -1.f : 1.f,
                    (i & (1 << 2)) ? -1.f : 1.f,
                    1.f
                );
                
                std::cout << corner.x << "\t" << corner.y << "\t" << corner.z << "\t" << corner.w << std::endl;
    }
    
}
void OverworldGameLayer::unloadSun() {
    glDeleteTextures(4, mSky.sunDepthTextures);
    glDeleteFramebuffers(4, mSky.sunFramebuffers);
    mSky.sunModel->drop();
}

void OverworldGameLayer::renderFrame(glm::vec4 debugShow, bool wireframe) {
    // Calculate shadow map cascades:
    {
        mSky.sunBasicViewMatrix = glm::lookAt(mSky.sunPosition - mSky.sunDirection, mSky.sunPosition, glm::vec3(0.f, 1.f, 0.f));
        
        glm::mat4 sunMatr = mSky.sunBasicViewMatrix;
        
        for(uint8_t i = 0; i < PGG_NUM_SUN_CASCADES; ++ i) {
            
            glm::mat4 projMatrix = glm::perspective(mCamera.fov, mCamera.aspect, mCamera.cascadeBorders[i], mCamera.cascadeBorders[i + 1]);
            glm::mat4 invVPMatrix = glm::inverse(projMatrix * mCamera.viewMat);
            
            glm::vec3 minBB;
            glm::vec3 maxBB;
            for(uint8_t j = 0; j < 8; ++ j) {
                glm::vec4 corner(
                    (j & (1 << 0)) ? -1.f : 1.f,
                    (j & (1 << 1)) ? -1.f : 1.f,
                    (j & (1 << 2)) ? -1.f : 1.f,
                    1.f
                );
                
                glm::vec4 cornerWorldSpace = invVPMatrix * corner;
                cornerWorldSpace /= cornerWorldSpace.w; // Perspective divide
                
                // DEBUG
                if(j < 4) {
                    //mTestCubes[(i * 4) + j]->setLocalTranslation(glm::vec3(cornerWorldSpace));
                }
                
                // cornerWorldSpace is a coordinate in world space for this corner of the view fustrum
                
                // find the location in "sunspace"
                glm::vec4 locInSun = sunMatr * cornerWorldSpace;
                
                if(j == 0) {
                    minBB = glm::vec3(locInSun);
                    maxBB = glm::vec3(locInSun);
                } else {
                    if(locInSun.x < minBB.x) { minBB.x = locInSun.x; }
                    if(locInSun.y < minBB.y) { minBB.y = locInSun.y; }
                    if(locInSun.z < minBB.z) { minBB.z = locInSun.z; }
                    if(locInSun.x > maxBB.x) { maxBB.x = locInSun.x; }
                    if(locInSun.y > maxBB.y) { maxBB.y = locInSun.y; }
                    if(locInSun.z > maxBB.z) { maxBB.z = locInSun.z; }
                }
            }
            
            mSky.sunViewMatrices[i] = mSky.sunBasicViewMatrix;
            mSky.sunProjMatrices[i] = glm::ortho(minBB.x, maxBB.x, minBB.y, maxBB.y, -50.f, 50.f);
        }
    }
    
    // Sunlight shadow pass
    {
        // Debugging
        /*
        {
            mSky.sunViewMatrices[0] = glm::lookAt(mSky.sunPosition - mSky.sunDirection, mSky.sunPosition, glm::vec3(0.f, 1.f, 0.f));
            mSky.sunProjMatrices[0] = glm::ortho(-10, 10, -10, 10, -10, 10);
            
            glViewport(0, 0, mSky.sunTextureSize, mSky.sunTextureSize);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSky.sunFramebuffers[0]);
            glDepthMask(GL_TRUE);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glDisable(GL_BLEND);
            glClear(GL_DEPTH_BUFFER_BIT);
            
            Model::RenderPassConfiguration sunRPC(Model::RenderPassType::SHADOW);
            sunRPC.viewMat = mSky.sunViewMatrices[0];
            sunRPC.projMat = mSky.sunProjMatrices[0];
            sunRPC.camPos = mSky.sunDirection * 100000.f;
            mRootNode->render(sunRPC);
        }
        */
        
        for(uint8_t i = 0; i < PGG_NUM_SUN_CASCADES; ++ i) {
            glViewport(0, 0, mSky.sunTextureSize, mSky.sunTextureSize);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSky.sunFramebuffers[i]);
            glDepthMask(GL_TRUE);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glDisable(GL_BLEND);
            glClear(GL_DEPTH_BUFFER_BIT);
            
            Model::RenderPass sunRPC(Model::RenderPassType::SHADOW);
            sunRPC.viewMat = mSky.sunViewMatrices[i];
            sunRPC.projMat = mSky.sunProjMatrices[i];
            sunRPC.camPos = mSky.sunDirection * 100000.f;
            mRootNode->render(sunRPC);
        }
    }
    
    // Debug
    //mCamera.viewMat = glm::lookAt(mSky.sunPosition - mSky.sunDirection, mSky.sunPosition, glm::vec3(0.f, 1.f, 0.f));
    //mCamera.projMat = glm::ortho(-10, 10, 10, -10, -100, 100);
    
    // Geometry pass
    {
        glViewport(0, 0, mScreenWidth, mScreenHeight);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBuff.gFramebuffer);
        GLuint colorAttachments[] = {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1
        };
        glDrawBuffers(2, colorAttachments);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDisable(GL_STENCIL_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glDisable(GL_BLEND);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if(wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        
        Model::RenderPass rootNodeRPC(Model::RenderPassType::GEOMETRY);
        rootNodeRPC.viewMat = mCamera.viewMat;
        rootNodeRPC.projMat = mCamera.projMat;
        rootNodeRPC.camPos = mCamera.position;
        mRootNode->render(rootNodeRPC);
        /*
        SceneNodeEComp* comp = (SceneNodeEComp*) mPlayerEntity->getComponent(SceneNodeEComp::sComponentID);
        mInfCheck->setFocus(comp->mSceneNode->getLocalTranslation());
        mInfCheck->render(rootNodeRPC, glm::mat4());
        */
        // mTerrainModel->render(rootNodeRPC, glm::mat4());
    }
    
    // Brightness Render
    {
        // Clear brightness
        {
            GLuint colorAttachment[] = {
                GL_COLOR_ATTACHMENT2
            };
            glDrawBuffers(1, colorAttachment);
            // Ambient light
            glClearColor(0.2f, 0.2f, 0.2f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);
        }
        
        // Do not write to the depth buffer
        glDepthMask(GL_FALSE);
        
        // Filled polygons
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        // glm::mat4 sunViewProjMat = mSky.sunBasicProjectionMatrix * mSky.sunBasicViewMatrix;
        
        // Render pass config
        Model::RenderPass brightRPC(Model::RenderPassType::LOCAL_LIGHTS);
        brightRPC.viewMat = mCamera.viewMat;
        brightRPC.projMat = mCamera.projMat;
        brightRPC.camPos = mCamera.position;
        // TODO: something else
        for(uint8_t i = 0; i < PGG_NUM_SUN_CASCADES + 1; ++ i) {
            brightRPC.cascadeBorders[i] = mCamera.cascadeBorders[i];
        }
        brightRPC.depthStencilTexture = mGBuff.depthStencilTexture;
        brightRPC.normalTexture = mGBuff.normalTexture;
        for(uint8_t i = 0; i < PGG_NUM_SUN_CASCADES; ++ i) {
            brightRPC.sunViewProjMatr[i] = mSky.sunProjMatrices[i] * mSky.sunViewMatrices[i];
            brightRPC.sunDepthTexture[i] = mSky.sunDepthTextures[i];
        }
        
        // Render local lights
        mRootNode->render(brightRPC);
        
        // Render global lights
        {
            // Prepare stencil
            // This also fills the diffuse buffer with white where the sky is
            {
                glDrawBuffer(GL_COLOR_ATTACHMENT0);
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(GL_LESS);
                glEnable(GL_STENCIL_TEST);
                glDisable(GL_CULL_FACE);
                glClearStencil(1);
                glClear(GL_STENCIL_BUFFER_BIT);
                
                glStencilFunc(GL_ALWAYS, 0, 0);
                
                // 1 = sky
                // 0 = ground
                glStencilOpSeparate(GL_FRONT_AND_BACK, GL_KEEP, GL_ZERO, GL_KEEP);
                
                glUseProgram(mSkyStencilShader.shaderProg->getHandle());
                
                glBindVertexArray(mFullscreenVao);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);
                
                glUseProgram(0);
                
                // Only keep pixels that are not a part of the sky
                glStencilFunc(GL_EQUAL, 0, 0xff);
            }
            
            // Prepare blending
            {
                GLuint colorAttachments[] = {
                    GL_COLOR_ATTACHMENT2
                };
                glDrawBuffers(1, colorAttachments);
                
                glDisable(GL_DEPTH_TEST);
                glDisable(GL_CULL_FACE);
                glEnable(GL_BLEND);
                glBlendEquation(GL_FUNC_ADD);
                glBlendFunc(GL_ONE, GL_ONE);
            }
            
            // Actual rendering
            {    
                brightRPC.type = Model::RenderPassType::GLOBAL_LIGHTS;
                mRootNode->render(brightRPC);
                mSky.sunModel->render(brightRPC, glm::inverse(mSky.sunBasicViewMatrix));
            }
        }
    }
    
    // Sky render
    {
        // Note: this makes use of the stencil created during the global brightness render
        
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glEnable(GL_STENCIL_TEST);
        
        // Only keep pixels that are a part of the sky
        glStencilFunc(GL_EQUAL, 1, 0xff);
        
        // Populate the brightness buffer
        glDrawBuffer(GL_COLOR_ATTACHMENT2);
        
        glUseProgram(mFillScreenShader.shaderProg->getHandle());
        
        glUniform3fv(mFillScreenShader.colorHandle, 1, glm::value_ptr(glm::vec3(0.0, 2.0, 2.0)));
        
        glBindVertexArray(mFullscreenVao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        glUseProgram(0);
        
    }
    
    // Screen render
    glViewport(0, 0, mScreenWidth, mScreenHeight);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDepthFunc(GL_EQUAL);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    /*
    if(debugShow != glm::vec4(0.f)) {
        glUseProgram(mDebugScreenShader.shaderProg->getHandle());
        
        glm::mat4 invViewProjMat = glm::inverse(mCamera.projMat * mCamera.viewMat);
        glUniformMatrix4fv(mDebugScreenShader.shaderProg->getInvViewProjMatrixUnif(), 1, GL_FALSE, glm::value_ptr(invViewProjMat));
        glUniform4fv(mDebugScreenShader.viewHandle, 1, glm::value_ptr(debugShow));
        
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, mGBuff.diffuseTexture);
        glUniform1i(mDebugScreenShader.diffuseHandle, 0);
        
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, mGBuff.normalTexture);
        glUniform1i(mDebugScreenShader.normalHandle, 1);
        
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, mGBuff.brightTexture);
        glUniform1i(mDebugScreenShader.brightHandle, 2);
        
        glActiveTexture(GL_TEXTURE0 + 3);
        glBindTexture(GL_TEXTURE_2D, mGBuff.depthStencilTexture);
        glUniform1i(mDebugScreenShader.depthHandle, 3);
        
        glBindVertexArray(mFullscreenVao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glBindVertexArray(0);
        
        glUseProgram(0);
        
    } else {
        */
    if(debugShow != glm::vec4(0.f)) {
        glUseProgram(mDebugSunShader.shaderProg->getHandle());
        
        glUniform4fv(mDebugSunShader.viewHandle, 1, glm::value_ptr(debugShow));
        
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, mSky.sunDepthTextures[0]);
        glUniform1i(mDebugSunShader.mapHandles[0], 0);
        
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, mSky.sunDepthTextures[1]);
        glUniform1i(mDebugSunShader.mapHandles[1], 1);
        
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, mSky.sunDepthTextures[2]);
        glUniform1i(mDebugSunShader.mapHandles[2], 2);
        
        glActiveTexture(GL_TEXTURE0 + 3);
        glBindTexture(GL_TEXTURE_2D, mSky.sunDepthTextures[3]);
        glUniform1i(mDebugSunShader.mapHandles[3], 3);
        
        glBindVertexArray(mFullscreenVao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glBindVertexArray(0);
        
        glUseProgram(0);
        
    } else {
        glUseProgram(mScreenShader.shaderProg->getHandle());
        
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, mGBuff.diffuseTexture);
        glUniform1i(mScreenShader.diffuseHandle, 0);
        
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTexture(GL_TEXTURE_2D, mGBuff.brightTexture);
        glUniform1i(mScreenShader.brightHandle, 1);
        
        glBindVertexArray(mFullscreenVao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        glUseProgram(0);
    }
}

bool OverworldGameLayer::onMouseMove(const SDL_MouseMotionEvent& event) {
    // float x = event.x;
    // float y = event.y;
    float dx = event.xrel;
    float dy = event.yrel;
    
    mCamYawNode->rotateYaw(-dx * 0.003f);
    mCamPitchNode->rotatePitch(-dy * 0.003f);
    
    return true;
}

bool OverworldGameLayer::onWindowSizeUpdate(const SDL_WindowEvent& event) {
    std::cout << event.data1 << ", " << event.data2 << std::endl;
    
    return true;
}
}

