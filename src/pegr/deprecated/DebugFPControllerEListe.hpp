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

#ifndef PEGR_DEBUGFPCONTROLLERELISTE_HPP
#define PEGR_DEBUGFPCONTROLLERELISTE_HPP

#include "pegr/deprecated/NRES.hpp"

#include "pegr/math/Vec3.hpp"

namespace pegr {

/*
 * Used in place of a normal physics component
 */
 
class DebugFPControllerEListe : public nres::Listener {
public:
    DebugFPControllerEListe(Vec3 initialLocation = Vec3());
    ~DebugFPControllerEListe();
    
    Vec3 mAbsoluteLocation;
public:
    void onEntityExists(nres::Entity* entity);
    void onEntityDestroyed(nres::Entity* entity);
    void onEntityBroadcast(nres::Entity* entity, const ESignal* data);

};

}

#endif // PEGR_DEBUGFPCONTROLLERELISTE_HPP
