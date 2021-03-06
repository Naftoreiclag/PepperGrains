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

#ifndef PEGR_SOUND_SOURCE_HPP
#define PEGR_SOUND_SOURCE_HPP

#include <stdint.h>
#include <vector>
#include <map>

#include "pegr/math/Vec3.hpp"
#include "pegr/misc/ReferenceCounted.hpp"
#include "pegr/sound/SoundWaveform.hpp"

#include "pegr/sound/SoundEndpoint.hpp"

namespace pegr {
namespace Sound {

/* Analogous to a physical speaker with a location, orientation, etc. During final
 * mixing, Contexts query all Sources to build a list of samples which are then passed
 * to the Endpoint. 
 */
class Source : public ReferenceCounted {
public:
    struct Modifier {
        typedef uint16_t Flag;
        
        // 3D effects:
        static const Flag DOPPLER; // Pitch affected by velocity
        static const Flag FALLOFF; // Energy loss due to distance
        static const Flag ATTENUATION; // Energy loss due to medium traveled through
        static const Flag OCCLUSION; // Changes due to occluding objects
        static const Flag FREQUENCY_AWARE_ABSORPTION; // Different frequencies are absorbed differently by an occluding object
        static const Flag MACH_DELAY; // Sound does not travel instantly, delay determined by mach speed and distance
        
        // Conjugate flags:
        static const Flag ALL_3D_MODIFIERS; // Enables all 3D effects
        static const Flag ALL; // Enables everything
        
        // No effects / default
        static const Flag NONE; // Enables nothing
    };
    
    Source(Modifier::Flag flags = Modifier::ALL);
    ~Source();
    
    void load();
    void unload();
    
    Modifier::Flag mEnabledModifiers;
    Vec3 mLocation;
    Vec3 mVelocity; // Instantaneous velocity, units per sec
};

} // namespace Sound
} // namespace pegr

#endif // PEGR_SOUND_SOURCE_HPP
