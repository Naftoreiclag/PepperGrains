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

#ifndef PEGR_TESSMODEL_HPP
#define PEGR_TESSMODEL_HPP

#include "pegr/video/Model.hpp"
#include "pegr/video/GeometryResource.hpp"
#include "pegr/video/ShaderProgramResource.hpp"
#include <OpenGLStuff.hpp>
#include "pegr/video/TextureResource.hpp"

namespace pegr {

class TessModel : public Model {
private:
    GLuint mVertexArrayObject;
    GLuint mVertexBufferObject;
    GLuint mIndexBufferObject;
    uint32_t mVertexSize;
    uint32_t mNumVertices;
    uint32_t mNumQuads;
    ShaderProgramResource* mShaderProg;
    GLuint mOffsetHandle;
    GLuint mInstanceBuffer;
    TextureResource* mHeightTexture;
    
    
    GLuint mCamPos;
    GLuint mMinTess;
    GLuint mMaxTess;
    GLuint mMinDist;
    GLuint mMaxDist;
    GLuint mHeightMap;
public:
    TessModel();
    virtual ~TessModel();

    void load();
    void unload();

    void render(Renderable::Pass rendPass, const glm::mat4& modelMat);

};

}


#endif // PGG_TessModel
