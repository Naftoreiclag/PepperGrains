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

#include "pegr/resources/ResourcesUtil.hpp"

#include "pegr/video/ImageResource.hpp"
#include "pegr/video/MaterialResource.hpp"
#include "pegr/misc/MiscResource.hpp"
#include "pegr/video/ModelResource.hpp"
#include "pegr/resources/Resource.hpp"
#include "pegr/misc/StringResource.hpp"
#include "pegr/video/TextureResource.hpp"
#include "pegr/video/GeometryResource.hpp"
#include "pegr/video/ShaderResource.hpp"
#include "pegr/video/ShaderProgramResource.hpp"
#include "pegr/video/FontResource.hpp"
#include "pegr/scripts/ScriptResource.hpp"
#include "pegr/misc/Logger.hpp"

namespace pegr {
namespace Resources {
    
    void populateResourceMap(ResourceMap& sResources, const Json::Value& resourcesData, boost::filesystem::path dataPackDir) {
        for(Json::Value::const_iterator iter = resourcesData.begin(); iter != resourcesData.end(); ++ iter) {
            const Json::Value& resourceData = *iter;
            
            std::string resType = resourceData["type"].asString();
            std::string name = resourceData["name"].asString();
            std::string file = resourceData["file"].asString();
            uint32_t size = resourceData["size"].asInt();
            
            Resource* newRes;
            if(resType == "string") {
                newRes = new StringResource();
            } else if(resType == "compute-shader") {
                newRes = new ShaderResource(ShaderResource::Type::COMPUTE);
            } else if(resType == "vertex-shader") {
                newRes = new ShaderResource(ShaderResource::Type::VERTEX);
            } else if(resType == "tess-control-shader") {
                newRes = new ShaderResource(ShaderResource::Type::TESS_CONTROL);
            } else if(resType == "tess-evaluation-shader") {
                newRes = new ShaderResource(ShaderResource::Type::TESS_EVALUATION);
            } else if(resType == "geometry-shader") {
                newRes = new ShaderResource(ShaderResource::Type::GEOMETRY);
            } else if(resType == "fragment-shader") {
                newRes = new ShaderResource(ShaderResource::Type::FRAGMENT);
            } else if(resType == "shader-program") {
                newRes = new ShaderProgramResource();
            } else if(resType == "image") {
                newRes = new ImageResource();
            } else if(resType == "texture") {
                newRes = new TextureResource();
            } else if(resType == "model") {
                newRes = new ModelResource();
            } else if(resType == "material") {
                newRes = new MaterialResource();
            } else if(resType == "geometry") {
                newRes = new GeometryResource();
            } else if(resType == "font") {
                newRes = new FontResource();
            } else if(resType == "script") {
                newRes = new ScriptResource();
            } else {
                newRes = new MiscResource();
            }
            
            newRes->setName(name);
            newRes->setFile(dataPackDir / file);
            newRes->setSize(size);
            
            std::pair<ResourceMap::iterator, bool> success = sResources.insert(std::make_pair(name, newRes));
            if(!success.second) {
                Logger::log(Logger::WARN)
                    << "Multiple resources with name: " << name << std::endl
                    << "Conficts with " << success.first->second->getName() << std::endl;
            }
        }
        
        Logger::log(Logger::INFO) << "Successfully loaded resources from: " << dataPackDir << std::endl;
    }
}
}

