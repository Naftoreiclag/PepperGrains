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

#ifndef PEGR_MISSIONGAMELAYER_HPP
#define PEGR_MISSIONGAMELAYER_HPP

#include "pegr/misc/GameLayer.hpp"
#include "pegr/video/ShoRenderer.hpp"
#include "pegr/video/SimpleScenegraph.hpp"

namespace pegr {

class MissionGameLayer : public Gamelayer {
public:
    MissionGameLayer();
    ~MissionGameLayer();
private:
    uint32_t mScreenWidth;
    uint32_t mScreenHeight;
    
    ShoRenderer* mRenderer = nullptr;
    SimpleScenegraph* mScenegraph = nullptr;
    
    float mPeriod;

public:
    // Lifecycle
    void onBegin();
    void onEnd();
    
    // Ticks
    void onTick(double tpf, const InputState* keyStates);
    
    bool onNeedRebuildRenderPipeline();

    //bool onQuit(const QuitEvent& event);
};

}

#endif // PEGR_MISSIONGAMELAYER_HPP
