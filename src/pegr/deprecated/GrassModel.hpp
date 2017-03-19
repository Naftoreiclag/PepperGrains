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

#ifndef PEGR_GRASSMODEL_HPP
#define PEGR_GRASSMODEL_HPP

#include "pegr/video/Model.hpp"
#include "pegr/video/GeometryResource.hpp"
#include "pegr/video/ShaderProgramResource.hpp"
#include "pegr/video/TextureResource.hpp"
#include <OpenGLStuff.hpp>

namespace pegr {

class GrassModel : public Model {
private:
    GLuint mVertexArrayObject;
    ShaderProgramResource* mShaderProg;
    GeometryResource* mGeometry;
    GLuint mOffsetHandle;
    GLuint mColorHandle;
    GLuint mOffsetsBuffer;
    GLuint mColorsBuffer;
    GLuint mDiffuseHandle;
    GLuint mHeightHandle;
    TextureResource* mDiffuseTexture;
    TextureResource* mHeightTexture;
public:
    GrassModel();
    virtual ~GrassModel();

    void load();
    void unload();

    void render(Renderable::Pass rendPass, const glm::mat4& modelMat);

};

}


#endif // PEGR_GRASSMODEL_HPP
