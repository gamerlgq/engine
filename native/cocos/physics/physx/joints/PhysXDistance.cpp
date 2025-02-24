/****************************************************************************
 Copyright (c) 2020-2022 Xiamen Yaji Software Co., Ltd.

 http://www.cocos.com

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.

 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
****************************************************************************/

#include "physics/physx/joints/PhysXDistance.h"
#include "math/Quaternion.h"
#include "physics/physx/PhysXSharedBody.h"
#include "physics/physx/PhysXUtils.h"

namespace cc {
namespace physics {

void PhysXDistance::onComponentSet() {
    _mJoint = PxDistanceJointCreate(PxGetPhysics(), &getTempRigidActor(), physx::PxTransform{physx::PxIdentity}, nullptr, physx::PxTransform{physx::PxIdentity});
}

void PhysXDistance::setPivotA(float x, float y, float z) {
    _mPivotA = physx::PxVec3{x, y, z};
    updatePose();
}

void PhysXDistance::setPivotB(float x, float y, float z) {
    _mPivotB = physx::PxVec3{x, y, z};
    updatePose();
}

void PhysXDistance::updateScale0() {
    updatePose();
}

void PhysXDistance::updateScale1() {
    updatePose();
}

void PhysXDistance::updatePose() {
    physx::PxTransform pose0{physx::PxIdentity};
    physx::PxTransform pose1{physx::PxIdentity};
    auto *node0 = _mSharedBody->getNode();
    node0->updateWorldTransform();
    pose0.p = _mPivotA * node0->getWorldScale();
    _mJoint->setLocalPose(physx::PxJointActorIndex::eACTOR0, pose0);
    if (_mConnectedBody) {
        auto *node1 = _mConnectedBody->getNode();
        node1->updateWorldTransform();
        pose1.p = _mPivotB * node1->getWorldScale();
    } else {
        pose1.p = _mPivotA * node0->getWorldScale();
        pose1.p += _mPivotB + node0->getWorldPosition();
        const auto &wr = node0->getWorldRotation();
        pose1.q *= physx::PxQuat{wr.x, wr.y, wr.z, wr.w};
    }
    _mJoint->setLocalPose(physx::PxJointActorIndex::eACTOR1, pose1);
}

} // namespace physics
} // namespace cc
