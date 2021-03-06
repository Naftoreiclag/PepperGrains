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

#ifndef PEGR_MODELRESOURCE_HPP
#define PEGR_MODELRESOURCE_HPP

#include "pegr/video/Geometry.hpp"
#include "pegr/video/Material.hpp"
#include "pegr/video/Model.hpp"
#include "pegr/resource/Resource.hpp"

namespace pegr {

class ModelResource : public Resource, public Model {
private:
    bool mLoaded;

    Geometry* mGeometry;
    Material* mMaterial;
    GLuint mVertexArrayObject;
    
    void loadError();
    void unloadError();
    bool mIsErrorResource;
public:
    ModelResource();
    virtual ~ModelResource();
public:
    void load();
    void unload();
    
    // Attempts to convert a resource into a model. On failure, return a fallback model.
    // Return type not guaranteed to be ModelResource.
    static Model* gallop(Resource* resource);

    // Render this model with the provided matrices
    //void render(Renderable::Pass rendPass, const glm::mat4& modelMat);
    
    Geometry* getGeometry() const;
    Material* getMaterial() const;
    
    #ifdef PGG_OPENGL
    void bindVertexArray();
    #endif
};

}

#endif // MODELRESOURCE_HPP
