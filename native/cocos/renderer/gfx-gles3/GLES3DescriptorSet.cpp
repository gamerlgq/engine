/****************************************************************************
 Copyright (c) 2019-2022 Xiamen Yaji Software Co., Ltd.

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

#include "GLES3Std.h"

#include "GLES3Buffer.h"
#include "GLES3Commands.h"
#include "GLES3DescriptorSet.h"
#include "GLES3DescriptorSetLayout.h"
#include "GLES3Texture.h"
#include "gfx-gles3/GLES3GPUObjects.h"
#include "states/GLES3Sampler.h"

namespace cc {
namespace gfx {

GLES3DescriptorSet::GLES3DescriptorSet() {
    _typedID = generateObjectID<decltype(this)>();
}

GLES3DescriptorSet::~GLES3DescriptorSet() {
    destroy();
}

void GLES3DescriptorSet::doInit(const DescriptorSetInfo & /*info*/) {
    const GLES3GPUDescriptorSetLayout *gpuDescriptorSetLayout = static_cast<GLES3DescriptorSetLayout *>(_layout)->gpuDescriptorSetLayout();
    const size_t descriptorCount = gpuDescriptorSetLayout->descriptorCount;
    const size_t bindingCount = gpuDescriptorSetLayout->bindings.size();

    _gpuDescriptorSet = ccnew GLES3GPUDescriptorSet;
    _gpuDescriptorSet->gpuDescriptors.resize(descriptorCount);
    for (size_t i = 0U, k = 0U; i < bindingCount; i++) {
        const DescriptorSetLayoutBinding &binding = gpuDescriptorSetLayout->bindings[i];
        for (uint32_t j = 0; j < binding.count; j++, k++) {
            _gpuDescriptorSet->gpuDescriptors[k].type = binding.descriptorType;
        }
    }

    _gpuDescriptorSet->descriptorIndices = &gpuDescriptorSetLayout->descriptorIndices;
}

void GLES3DescriptorSet::doDestroy() {
    CC_SAFE_DELETE(_gpuDescriptorSet);
}

void GLES3DescriptorSet::update() {
    if (_isDirty && _gpuDescriptorSet) {
        auto &descriptors = _gpuDescriptorSet->gpuDescriptors;
        for (size_t i = 0; i < descriptors.size(); i++) {
            if (hasAnyFlags(descriptors[i].type, DESCRIPTOR_BUFFER_TYPE)) {
                if (_buffers[i].ptr) {
                    descriptors[i].gpuBuffer = static_cast<GLES3Buffer *>(_buffers[i].ptr)->gpuBuffer();
                }
            } else if (hasAnyFlags(descriptors[i].type, DESCRIPTOR_TEXTURE_TYPE)) {
                if (_textures[i].ptr) {
                    _gpuDescriptorSet->gpuDescriptors[i].gpuTextureView = static_cast<GLES3Texture *>(_textures[i].ptr)->gpuTextureView();
                }
                if (_samplers[i].ptr) {
                    descriptors[i].gpuSampler = static_cast<GLES3Sampler *>(_samplers[i].ptr)->gpuSampler();
                }
            }
        }
        _isDirty = false;
    }
}

void GLES3DescriptorSet::forceUpdate() {
    _isDirty = true;
    update();
}

} // namespace gfx
} // namespace cc
