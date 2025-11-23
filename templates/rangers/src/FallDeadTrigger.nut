class FallDeadTrigger {
	static function AddDebugVisual(obj, debugVisual) {
		local volume = obj.parameters().GetParameterObject("volume");
		local objposition = obj.position();
		local volumeoffset = volume.GetParameterArray("CollisionOffset");
		local position = Vec3(objposition.x() + volumeoffset[0], objposition.y() + volumeoffset[1], objposition.z() + volumeoffset[2]);
		local rotation = Vec4(0, 0, 0, 1);
		local scale = Vec3(volume.GetParameterFloat("CollisionWidth"), volume.GetParameterFloat("CollisionHeight"), volume.GetParameterFloat("CollisionDepth"));
		local color = Vec4(1, 0, 0, 0.2);
		debugVisual.DrawCube(color, position, rotation, scale, obj);
	}
}
