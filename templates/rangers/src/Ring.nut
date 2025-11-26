class Ring {
    static function GetModelData(obj, modelData) {
        local model = HLModelLoadFilepath("E:\\Steam\\steamapps\\common\\SonicFrontiers\\image\\x64\\raw\\CommonObject\\cmn_obj_ring\\cmn_obj_ring.0.model");
        ModelDataAssign(modelData, model.GetModelData());
    }
}
