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

#ifndef PEGR_MODEL_HPP
#define PEGR_MODEL_HPP

#include "pegr/misc/ReferenceCounted.hpp"
#include "pegr/resource/Resource.hpp"
#include "pegr/video/Renderable.hpp"
#include "pegr/video/Geometry.hpp"
#include "pegr/video/Material.hpp"

namespace pegr {

// Virtual inheritance to avoid diamond conflict with ModelResource
class Model : virtual public ReferenceCounted {
protected:
    Geometry* mGeometry;
    Material* mMaterial;
    
public:
    static Model* getFallback();
    
    Model();
    Model(Geometry* geometry, Material* model);
    virtual ~Model();
    
    virtual Geometry* getGeometry() const;
    virtual Material* getMaterial() const;
    
    virtual void load();
    virtual void unload();
    
    #ifdef PGG_OPENGL
    virtual void bindVertexArray();
    #endif
};

}

#endif // PEGR_MODEL_HPP
