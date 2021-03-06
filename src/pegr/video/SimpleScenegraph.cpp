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

#include "pegr/video/SimpleScenegraph.hpp"

namespace pegr {
// Perform a function on all attached ModelInstances
void SimpleScenegraph::processAll(std::function<void(ModelInstance*)> modelMapper) {
    if(mModelInst) {
        modelMapper(mModelInst);
    }
}

// Provide a function to be called when a new ModelInstance is added
void SimpleScenegraph::setRendererAddListener(std::function<void(ModelInstance*)> modelFunc) {
    mAddListener = modelFunc;
}

// Provide a function to be called when a ModelInstance is removed
void SimpleScenegraph::setRendererRemoveListener(std::function<void(ModelInstance*)> modelFunc) {
    mRemoveListener = modelFunc;
}

ModelInstance* SimpleScenegraph::setModelInstance(ModelInstance* modelInst) {
    if(mModelInst == modelInst) return mModelInst;
    if(mModelInst != nullptr) mRemoveListener(mModelInst);
    ModelInstance* oldModelInstance = mModelInst;
    mModelInst = modelInst;
    if(mModelInst != nullptr) mAddListener(mModelInst);
    return oldModelInstance;
}

}

