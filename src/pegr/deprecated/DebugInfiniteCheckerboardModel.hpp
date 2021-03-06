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

#ifndef PEGR_DEBUGINFINITECHECKERBOARDMODEL_HPP
#define PEGR_DEBUGINFINITECHECKERBOARDMODEL_HPP

#include "pegr/math/Vec3.hpp"
#include "pegr/video/Model.hpp"
#include "pegr/video/ShaderProgramResource.hpp"

namespace pegr {

class InfiniteCheckerboardModel : public Model {
private:
    GLuint mVertexArrayObject;
    GLuint mVertexBufferObject;
    GLuint mIndexBufferObject;
    ShaderProgramResource* mShaderProg;
    
    // Length of one edge in meters
    uint32_t mSize;
    float mCellSize;
    
    uint32_t mIndicesSize;
    
    Vec3 mFocus;
    
    
public:
    InfiniteCheckerboardModel();
    ~InfiniteCheckerboardModel();
    
    void load();
    void unload();
    
    void setFocus(Vec3 location);
    
    void render(Renderable::Pass rendPass, const glm::mat4& unused);
};

}

#endif // PEGR_DEBUGINFINITECHECKERBOARDMODEL_HPP
