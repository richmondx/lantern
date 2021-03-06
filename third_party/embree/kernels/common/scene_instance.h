// ======================================================================== //
// Copyright 2009-2015 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "accelset.h"

namespace embree
{
  class InstanceFactory
  {
  public:
    InstanceFactory(int features);
    DEFINE_SYMBOL2(RTCBoundsFunc2,InstanceBoundsFunc);
    DEFINE_SYMBOL2(AccelSet::Intersector1,InstanceIntersector1);
    DEFINE_SYMBOL2(AccelSet::Intersector4,InstanceIntersector4);
    DEFINE_SYMBOL2(AccelSet::Intersector8,InstanceIntersector8);
    DEFINE_SYMBOL2(AccelSet::Intersector16,InstanceIntersector16);
  };

  /*! Instanced acceleration structure */
  struct Instance : public AccelSet
  {
  public:
    Instance (Scene* parent, Accel* object, size_t numTimeSteps); 
    virtual void setTransform(const AffineSpace3fa& local2world, size_t timeStep);
    virtual void setMask (unsigned mask);
    virtual void build(size_t threadIndex, size_t threadCount) {}
    
  public:
    AffineSpace3fa local2world[2]; //!< transforms from local space to world space
    AffineSpace3fa world2local[2]; //!< transforms from world space to local space
    Accel* object;                 //!< pointer to instanced acceleration structure
  };
}
