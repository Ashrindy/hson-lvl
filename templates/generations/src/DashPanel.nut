function vertexBytes(l, x, y, z, u, v) {
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
    l.append(ToBytes(u)[0]);
    l.append(ToBytes(u)[1]);
    l.append(ToBytes(u)[2]);
    l.append(ToBytes(u)[3]);
    l.append(ToBytes(v)[0]);
    l.append(ToBytes(v)[1]);
    l.append(ToBytes(v)[2]);
    l.append(ToBytes(v)[3]);
}

class DashPanel {
    static test = 0;

    static function GetModelData(obj, modelData) {
        local vertices = [];

        vertexBytes(vertices, -0.5, -0.5,  0.5, 0.0, 0.0);
        vertexBytes(vertices,  0.5, -0.5,  0.5, 1.0, 0.0);
        vertexBytes(vertices,  0.5,  0.5,  0.5, 1.0, 1.0);
        vertexBytes(vertices, -0.5,  0.5,  0.5, 0.0, 1.0);

        vertexBytes(vertices,  0.5, -0.5, -0.5, 0.0, 0.0);
        vertexBytes(vertices, -0.5, -0.5, -0.5, 1.0, 0.0);
        vertexBytes(vertices, -0.5,  0.5, -0.5, 1.0, 1.0);
        vertexBytes(vertices,  0.5,  0.5, -0.5, 0.0, 1.0);

        vertexBytes(vertices, -0.5, -0.5, -0.5, 0.0, 0.0);
        vertexBytes(vertices, -0.5, -0.5,  0.5, 1.0, 0.0);
        vertexBytes(vertices, -0.5,  0.5,  0.5, 1.0, 1.0);
        vertexBytes(vertices, -0.5,  0.5, -0.5, 0.0, 1.0);

        vertexBytes(vertices,  0.5, -0.5,  0.5, 0.0, 0.0);
        vertexBytes(vertices,  0.5, -0.5, -0.5, 1.0, 0.0);
        vertexBytes(vertices,  0.5,  0.5, -0.5, 1.0, 1.0);
        vertexBytes(vertices,  0.5,  0.5,  0.5, 0.0, 1.0);

        vertexBytes(vertices, -0.5,  0.5,  0.5, 0.0, 0.0);
        vertexBytes(vertices,  0.5,  0.5,  0.5, 1.0, 0.0);
        vertexBytes(vertices,  0.5,  0.5, -0.5, 1.0, 1.0);
        vertexBytes(vertices, -0.5,  0.5, -0.5, 0.0, 1.0);

        vertexBytes(vertices, -0.5, -0.5, -0.5, 0.0, 0.0);
        vertexBytes(vertices,  0.5, -0.5, -0.5, 1.0, 0.0);
        vertexBytes(vertices,  0.5, -0.5,  0.5, 1.0, 1.0);
        vertexBytes(vertices, -0.5, -0.5,  0.5, 0.0, 1.0);

        local indices = [
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4,
            8, 9,10,10,11, 8,
            12,13,14,14,15,12,
            16,17,18,18,19,16,
            20,21,22,22,23,20
        ];

        modelData.SetVertexCount(vertices.len() / 20);
        modelData.SetVertexStride(20);

        modelData.SetVertices(vertices);

        modelData.SetIndexCount(indices.len());
        modelData.SetIndices(indices);
    }

    static function AddDynamicDebugVisual(obj, debugVisual, deltaTime) {
        local position = obj.position();
        local rotation = Vec4(0, 0, 0, 1);
        local scale = Vec3(50000, 100, 50000);
        local color = Vec4(1, 1, 0, 0.2);
        print(DashPanel.test + "\n");
        DashPanel.test <- DashPanel.test + deltaTime;
        debugVisual.DrawCube(color, position, rotation, scale, obj);
    }

    static function DynamicDebugVisualEnd(obj) {
        DashPanel.test <- 0;
    }
}
