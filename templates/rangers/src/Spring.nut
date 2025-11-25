class Spring {
	static function AddDebugVisual(obj, debugVisual) {
        local position = obj.position();
        local rotation = obj.rotation();
		local params = obj.parameters();
        local color = Vec4(1, 0, 1, 1)
		local points = [];
		local gravityDir = Vec3(0, -1, 0);
		local dt = 0.016666668;

		local timeElapsed = 0.0;
		local speed = params.GetParameterFloat("firstSpeed");
		local velocityDistance = 0.0;
		if (fabs(speed) > 0.000001)
			velocityDistance = params.GetParameterFloat("keepVelocityDistance") / speed;

		local outOfControl = params.GetParameterFloat("outOfControl");
		local outOfControlTime = fmax(outOfControl - velocityDistance, 0.0);

		local upVector0 = Vec3(0, 0.349999, 0);
		position = upVector0.multiplyMat3(obj.linear()) + position;
		local oldPosition = position;
		local up = Vec3(0, 1, 0);
		position = up.multiplyMat3(obj.linear());

		local speedDistance = position * speed;

		position = speedDistance * velocityDistance + gravityDir + (((gravityDir * 35.0 * outOfControlTime) + speedDistance * 2.0) * outOfControlTime * 0.5);

		local maxTime = outOfControlTime + velocityDistance;

		gravityDir = gravityDir * 35.0;

		local prevNewPos = speedDistance;
		local pos = oldPosition;
		local prevPos = oldPosition;

		points.append(pos);

		if (maxTime > 0) {
			while (timeElapsed < maxTime) {
				local newPos = Vec3(0, 0, 0);
				if (timeElapsed < velocityDistance) {
					newPos = prevNewPos;
				}
				else if (timeElapsed > outOfControl) {
					// Most of the time unused
					prevNewPos = prevNewPos + (gravityDir * dt);
					local gravityDirNorm = gravityDir.normalize();
					if (gravityDirNorm >= 0.0000000099999999) {
						local gravityDirNormSqrt = sqrt(gravityDirNorm);
						if (gravityDirNormSqrt != 0.0) {
							position = gravityDir / gravityDirNormSqrt;
						}
					}
					else {
						position = Vec3(0, 0, 0);
					}
					// TODO: Figure out that one func
				}
				else {
					newPos = prevNewPos + (gravityDir * dt);
					prevNewPos = newPos;
				}
				pos = pos + (newPos * dt);
				if (pos.distancesq(prevPos) >= 1.0) {
					points.append(pos);
					prevPos = pos;
				}
				timeElapsed = timeElapsed + dt;
			}
		}

		// TODO: Find a way to smooth out the line like the game (SNS I think)
		local finalPoints = [];

		for (local i = 0; i + 1 < points.len(); i++) {
			finalPoints.append(points[i]);
			finalPoints.append(points[i + 1]);
		}

        debugVisual.DrawLine(color, finalPoints, obj);
    }
}
