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

#ifndef PEGR_POINTLIGHTMODEL_HPP
#define PEGR_POINTLIGHTMODEL_HPP

#include "pegr/video/Model.hpp"
#include "pegr/video/GeometryResource.hpp"
#include "pegr/video/ShaderProgramResource.hpp"

namespace pegr {

class PointLightModel : public Model {
public:
    class SharedResources : public ReferenceCounted {
    private:
        GLuint mVertexArrayObject;
        ShaderProgramResource* mMinimalShader;
        ShaderProgramResource* mShaderProg;
        GLuint mNormalHandle;
        GLuint mDepthHandle;
        GLuint mPositionHandle;
        GLuint mColorHandle;
        GLuint mRadiusHandle;
        GLuint mVolumeRadiusHandle;
        GLuint mStencilVolumeRadiusHandle;
        GLuint mStencilPositionHandle;
        GeometryResource* mGeometry;
        
        SharedResources();
        ~SharedResources();
        
    public:
        static SharedResources* getSharedInstance();
        
        void load();
        void unload();
        void render(Renderable::Pass rendPass, const glm::mat4& modelMat, const glm::vec3& color, const GLfloat& radius, const GLfloat& volumeRadius);
    };
private:
    
    SharedResources* mSharedRes;
    
    glm::vec3 mColor;
    GLfloat mRadius;
    GLfloat mVolumeRadius;
    
    static float calcVolumeRadius(const glm::vec3& color, const float& radius);
public:
    PointLightModel(glm::vec3 brightness = glm::vec3(1.0), float radius = 1.f);
    virtual ~PointLightModel();

    void load();
    void unload();

    void render(Renderable::Pass rendPass, const glm::mat4& modelMat);
    
    void setBrightness(glm::vec3 brightness, float radius);
};

}

#endif // PEGR_POINTLIGHTMODEL_HPP
