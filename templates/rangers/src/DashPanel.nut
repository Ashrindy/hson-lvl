function vertexBytes(l, x, y, z) {
    l.append(ToBytes(x)[0]);
    l.append(ToBytes(x)[1]);
    l.append(ToBytes(x)[2]);
    l.append(ToBytes(x)[3]);
    l.append(ToBytes(y)[0]);
    l.append(ToBytes(y)[1]);
    l.append(ToBytes(y)[2]);
    l.append(ToBytes(y)[3]);
    l.append(ToBytes(z)[0]);
    l.append(ToBytes(z)[1]);
    l.append(ToBytes(z)[2]);
    l.append(ToBytes(z)[3]);
}

class DashPanel {
    static function GetModelData(obj, modelData) {
        /*local vertices = [];

        vertexBytes(vertices, -0.5, -0.5,  0.5);
        vertexBytes(vertices,  0.5, -0.5,  0.5);
        vertexBytes(vertices,  0.5,  0.5,  0.5);
        vertexBytes(vertices, -0.5,  0.5,  0.5);

        vertexBytes(vertices,  0.5, -0.5, -0.5);
        vertexBytes(vertices, -0.5, -0.5, -0.5);
        vertexBytes(vertices, -0.5,  0.5, -0.5);
        vertexBytes(vertices,  0.5,  0.5, -0.5);

        vertexBytes(vertices, -0.5, -0.5, -0.5);
        vertexBytes(vertices, -0.5, -0.5,  0.5);
        vertexBytes(vertices, -0.5,  0.5,  0.5);
        vertexBytes(vertices, -0.5,  0.5, -0.5);

        vertexBytes(vertices,  0.5, -0.5,  0.5);
        vertexBytes(vertices,  0.5, -0.5, -0.5);
        vertexBytes(vertices,  0.5,  0.5, -0.5);
        vertexBytes(vertices,  0.5,  0.5,  0.5);

        vertexBytes(vertices, -0.5,  0.5,  0.5);
        vertexBytes(vertices,  0.5,  0.5,  0.5);
        vertexBytes(vertices,  0.5,  0.5, -0.5);
        vertexBytes(vertices, -0.5,  0.5, -0.5);

        vertexBytes(vertices, -0.5, -0.5, -0.5);
        vertexBytes(vertices,  0.5, -0.5, -0.5);
        vertexBytes(vertices,  0.5, -0.5,  0.5);
        vertexBytes(vertices, -0.5, -0.5,  0.5);

        local indices = [
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4,
            8, 9,10,10,11, 8,
            12,13,14,14,15,12,
            16,17,18,18,19,16,
            20,21,22,22,23,20
        ];

        local posElem = VertexElement("POSITION", VertexFormat.R32G32B32_FLOAT);
        modelData.AddVertexElement(posElem);

        modelData.SetVertexCount(vertices.len() / modelData.GetVertexStride());
        modelData.SetVertices(vertices);

        modelData.SetIndexCount(indices.len());
        modelData.SetIndices(indices);*/

        local model = HLModelLoadFilepath("E:\\Steam\\steamapps\\common\\SonicFrontiers\\image\\x64\\raw\\stage\\CyberObject\\cmn_obj_dashpanel\\cmn_obj_dashpanel.0.model");
        ModelDataAssign(modelData, model.GetModelData());
    }

    static function AddDynamicDebugVisual(obj, debugVisual, deltaTime) {
        local position = obj.position();
        local rotation = Vec4(0, 0, 0, 1);
        local scale = Vec3(50000, 100, 50000);
        local color = Vec4(1, 1, 0, 0.2);
        debugVisual.DrawCube(color, position, rotation, scale, obj);
    }

    static function DynamicDebugVisualEnd(obj) {}
}
