class Spring {
	static function AddDebugVisual(obj, debugVisual) {
		print("test\n");
        local position = obj.position();
        local rotation = obj.rotation();
        local color = Vec4(1, 0, 1, 1)
		local points = [];
		local dtVec = Vec3(0.01666668, 0.01666668, 0.01666668);

		local start = Vec3(position.x(), position.y(), position.z());
		local end = Vec3(position.x() + 10, position.y() + 10, position.z() + 10);

		local params = obj.parameters();
		local speed = params.GetParameterFloat("firstSpeed");
		local outOfControl = params.GetParameterFloat("outOfControl");
		local accelTime = params.GetParameterFloat("keepVelocityDistance") / speed;
		local totalTime = accelTime + outOfControl;

		local pos = start;
		local lastStoredPos = pos;

		local dir = Vec3Sub(end, start);
		local distSq = Vec3LengthSq(dir);

		if (distSq > 0)
			dir = Vec3Divide(dir, Vec3(sqrt(distSq), sqrt(distSq), sqrt(distSq)));
		else
			dir = Vec3(0, 1, 0);

		local vel = Vec3Multiply(dir, Vec3(speed, speed, speed));

		local gravity = Vec3(0, -1, 0);

		local t = 0;

		while (t < totalTime) {
			if (t < accelTime) {
				vel = Vec3Add(vel, Vec3Multiply(dir, dtVec));
			}
			else if (t > outOfControl) {
				vel = Vec3Add(vel, Vec3Multiply(gravity, dtVec));

				local vlenSq = Vec3LengthSq(vel);

				if (vlenSq > 0) {
					local inv = 1 / sqrt(vlenSq);
					local vdir = Vec3Multiply(vel, Vec3(inv, inv, inv));
					vel = vdir;
				}
				else {
					vel = Vec3(0, 0, 0);
				}
			}
			else {
				vel = Vec3Add(vel, Vec3Multiply(gravity, dtVec));
			}

			pos = Vec3Add(pos, Vec3Multiply(vel, dtVec));

			local deltaPos = Vec3Sub(pos, lastStoredPos);
			if (Vec3LengthSq(deltaPos) >= 1) {
				points.append(lastStoredPos);
				points.append(pos);
				lastStoredPos = pos;
			}

			t = t + 0.01666668;
		}

		print("test\n");
        debugVisual.DrawLine(color, Vec3(0,0,0), Vec4(0,0,0,0), points, obj);
    }
}

function Vec3Add(v1, v2) {
    return Vec3(v1.x() + v2.x(), v1.y() + v2.y(), v1.z() + v2.z())
}

function Vec3Sub(v1, v2) {
    return Vec3(v1.x() - v2.x(), v1.y() - v2.y(), v1.z() - v2.z())
}

function Vec3Multiply(vecOne, vecTwo) {
    return Vec3(vecOne.x() * vecTwo.x(), vecOne.y() * vecTwo.y(), vecOne.z() * vecTwo.z())
}

function Vec3Divide(vecOne, vecTwo) {
    return Vec3(vecOne.x() / vecTwo.x(), vecOne.y() / vecTwo.y(), vecOne.z() / vecTwo.z())
}

function Vec3LengthSq(vec) {
	return vec.x()*vec.x() + vec.y()*vec.y() + vec.z()*vec.z();
}
