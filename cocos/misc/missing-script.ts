/*
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2020 Xiamen Yaji Software Co., Ltd.

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
*/

import { ccclass, inspector, editorOnly, serializable } from 'cc.decorator';
import { Component } from '../scene-graph/component';
import { warnID, error, js, cclegacy  } from '../core';

/**
 * @en
 * A temp fallback to contain the original component which can not be loaded.
 * @zh
 * 包含无法加载的原始组件的临时回退。
 */
@ccclass('cc.MissingScript')
@inspector('packages://inspector/inspectors/comps/missing-script.js')
export class MissingScript extends Component {
    // _scriptUuid: {
    //    get: function () {
    //        var id = this._$erialized.__type__;
    //        if (EditorExtends.UuidUtils.isUuid(id)) {
    //            return EditorExtends.UuidUtils.decompressUuid(id);
    //        }
    //        return '';
    //    },
    // },

    /*
     * @param {string} id
     * @return {function} constructor
     */
    public static safeFindClass (id: string) {
        const cls = js.getClassById(id);
        if (cls) {
            return cls;
        }
        cclegacy.deserialize.reportMissingClass(id);

        return undefined;
    }

    // the serialized data for original script object
    /**
     * @deprecated since v3.5.0, this is an engine private interface that will be removed in the future.
     */
    @serializable
    @editorOnly
    public _$erialized = null;

    constructor () {
        super();
    }

    public onLoad () {
        warnID(4600, this.node.name);
    }
}

cclegacy._MissingScript = MissingScript;

// DEBUG: Check MissingScript class for issue 9878
// import { error } from '../platform/debug';
try {
    const props = MissingScript.__values__;
    if (props.length === 0 || props[props.length - 1] !== '_$erialized') {
        error(`The '_$erialized' prop in MissingScript is missing. Please contact jare.`);
        error(`    Error props: ['${props}']`);
        // props.push('_$erialized');
    }
} catch (e) {
    error(`Error when checking MissingScript 5, ${e}`);
}
