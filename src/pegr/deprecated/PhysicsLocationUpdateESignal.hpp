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

#ifndef PEGR_PHYSICSLOCATIONUPDATEESYS_HPP
#define PEGR_PHYSICSLOCATIONUPDATEESYS_HPP

#include "pegr/deprecated/NRES.hpp"

#include "pegr/deprecated/EntitySignal.hpp"
#include "pegr/math/Vec3.hpp"

namespace pegr {

class PhysicsLocationUpdateESignal : public ESignal {
public:
    PhysicsLocationUpdateESignal(const Vec3& absLocation);
    ~PhysicsLocationUpdateESignal();
    
    const Vec3 mAbsoluteLocation;
    
    ESignal::Type getType() const;

};

}

#endif // PEGR_PHYSICSLOCATIONUPDATEESYS_HPP
