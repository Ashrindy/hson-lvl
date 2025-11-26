class JumpBoard {
    static function GetModelData(obj, modelData) {
        local type = obj.parameters().GetParameterString("size").c_str();
        local filepath = "";
        if (type == "SIZE_S") {
            filepath = "E:\\Steam\\steamapps\\common\\SonicFrontiers\\image\\x64\\raw\\stage\\CyberObject\\cmn_obj_jumppanel30S\\cmn_obj_jumppanel30S.0.model";
        }
        else if (type == "SIZE_M") {
            filepath = "E:\\Steam\\steamapps\\common\\SonicFrontiers\\image\\x64\\raw\\stage\\CyberObject\\cmn_obj_jumppanel30M\\cmn_obj_jumppanel30M.0.model";
        }
        else if (type == "SIZE_L") {
            filepath = "E:\\Steam\\steamapps\\common\\SonicFrontiers\\image\\x64\\raw\\stage\\CyberObject\\cmn_obj_jumppanel30L.model";
        }
        local model = HLModelLoadFilepath(filepath);
        ModelDataAssign(modelData, model.GetModelData());
    }
}
