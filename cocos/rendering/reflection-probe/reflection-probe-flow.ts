/*
 Copyright (c) 2020 Xiamen Yaji Software Co., Ltd.

 https://www.cocos.com/

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
 */
import { EDITOR } from 'internal:constants';
import { ccclass } from 'cc.decorator';
import { IRenderFlowInfo, RenderFlow } from '../render-flow';
import { ReflectionProbeStage } from './reflection-probe-stage';
import { RenderFlowTag } from '../pipeline-serialization';
import { RenderPipeline } from '..';
import { Camera, ProbeType, ReflectionProbe } from '../../render-scene/scene';
import { ReflectionProbeManager } from '../reflection-probe-manager';

/**
 * @en reflection probe render flow
 * @zh 反射探针rendertexture绘制流程
 */
@ccclass('ReflectionProbeFlow')
export class ReflectionProbeFlow extends RenderFlow {
    public static initInfo: IRenderFlowInfo = {
        name: 'PIPELINE_FLOW_RELECTION_PROBE',
        priority: 0,
        tag: RenderFlowTag.SCENE,
        stages: [],
    };

    public initialize (info: IRenderFlowInfo): boolean {
        super.initialize(info);
        if (this._stages.length === 0) {
            const probeStage = new ReflectionProbeStage();
            probeStage.initialize(ReflectionProbeStage.initInfo);
            this._stages.push(probeStage);
        }
        return true;
    }

    public activate (pipeline: RenderPipeline) {
        super.activate(pipeline);
    }

    public render (camera: Camera) {
        const probes = ReflectionProbeManager.probeManager.getProbes();
        for (let i = 0; i < probes.length; i++) {
            if (probes[i].needRender) {
                if (EDITOR || probes[i].probeType === ProbeType.PLANAR) {
                    this._renderStage(camera, probes[i]);
                }
            }
        }
    }

    public destroy () {
        super.destroy();
    }
    private _renderStage (camera: Camera, probe: ReflectionProbe) {
        for (let i = 0; i < this._stages.length; i++) {
            const probeStage = this._stages[i] as ReflectionProbeStage;
            if (probe.probeType === ProbeType.PLANAR) {
                ReflectionProbeManager.probeManager.updatePlanarMap(probe, null);
                probeStage.setUsageInfo(probe, probe.realtimePlanarTexture!.window!.framebuffer);
                probeStage.render(camera);
                ReflectionProbeManager.probeManager.updatePlanarMap(probe, probe.realtimePlanarTexture!.getGFXTexture());
            } else {
                for (let faceIdx = 0; faceIdx < 6; faceIdx++) {
                    //update camera dirction
                    probe.updateCameraDir(faceIdx);
                    const renderTexture = probe.bakedCubeTextures[faceIdx];
                    probeStage.setUsageInfo(probe, renderTexture.window!.framebuffer);
                    probeStage.render(camera);
                }
                probe.needRender = false;
            }
        }
    }
}
