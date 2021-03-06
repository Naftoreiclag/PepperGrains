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

#ifndef PEGR_TEXTURERESOURCE_HPP
#define PEGR_TEXTURERESOURCE_HPP

#include "pegr/video/GraphicsApiLibrary.hpp"

#include "pegr/resource/Resource.hpp"
#include "pegr/video/Image.hpp"
#include "pegr/video/Texture.hpp"

namespace pegr {

class TextureResource : public Texture, public Resource {
private:
    #ifdef PGG_OPENGL
    GLuint mHandle;
    #endif
    #ifdef PGG_VULKAN
    VkSampler mSamplerHandle = VK_NULL_HANDLE;
    #endif
    bool mLoaded;

    // (Not actually required to keep the image loaded)
    Image* mImage;
    
    void loadError();
public:
    TextureResource();
    ~TextureResource();
    
    static Texture* gallop(Resource* resource);
    
    void load();
    void unload();
    
    #ifdef PGG_OPENGL
    GLuint getHandle() const;
    #endif
    #ifdef PGG_VULKAN
    VkSampler getSamplerHandle() const;
    #endif // PGG_VULKAN
    
    Image* getImage() const;
};

}

#endif // TEXTURERESOURCE_HPP
