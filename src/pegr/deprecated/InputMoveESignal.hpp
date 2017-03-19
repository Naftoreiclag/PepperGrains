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

#ifndef PEGR_MOVEINPUTESIGNAL_HPP
#define PEGR_MOVEINPUTESIGNAL_HPP

#include "pegr/deprecated/EntitySignal.hpp"

#include "pegr/math/Vec3.hpp"

namespace pegr {

class InputMoveESignal : public ESignal {
public:
    InputMoveESignal(const Vec3& displacement);
    ~InputMoveESignal();
    
    const Vec3 mDisplacement;
    
    ESignal::Type getType() const;
};

}

#endif // PEGR_MOVEINPUTESIGNAL_HPP
