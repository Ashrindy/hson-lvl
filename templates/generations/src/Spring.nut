//Ported from GLVL
class HE1JumpCollisionHelper {
    static ms_TotalTimer = 0
    static ms_KeepVelocityDistance = 0
    static ms_GravityTimer = 0
    static ms_IsOutOfKeepVelocity = false

    static function GetTrajectoryGravity(
        in_FirstSpeed,
        in_KeepDistance,
        in_YDirection,
        total_time
    ) {
        local delta_y = 0.0
        local gravity = 35

        if (
            in_FirstSpeed * total_time > in_KeepDistance &&
            !HE1JumpCollisionHelper.ms_IsOutOfKeepVelocity
        ) {
            local temp_distance = in_FirstSpeed * total_time
            HE1JumpCollisionHelper.ms_KeepVelocityDistance <- temp_distance
            HE1JumpCollisionHelper.ms_GravityTimer <- 0.0
            HE1JumpCollisionHelper.ms_IsOutOfKeepVelocity <- true
        } else {
            if (!HE1JumpCollisionHelper.ms_IsOutOfKeepVelocity) {
                local temp_distance = in_FirstSpeed * total_time
                HE1JumpCollisionHelper.ms_KeepVelocityDistance <- temp_distance
            }
        }

        if (HE1JumpCollisionHelper.ms_IsOutOfKeepVelocity) {
            delta_y =
                in_FirstSpeed *
                    HE1JumpCollisionHelper.ms_GravityTimer *
                    in_YDirection +
                0.5 *
                    -gravity *
                    (HE1JumpCollisionHelper.ms_GravityTimer *
                        HE1JumpCollisionHelper.ms_GravityTimer)
        }

        return delta_y
    }
    // static function AddDebugVisual(obj, debugVisual)
    // {
    //     local position = obj.position()
    //     local rotation = obj.rotation()
    //     local color = Vec4(1, 0, 1, 1)
    //     local scale = Vec3(10, 10, 10)
    //     debugVisual.DrawLine(
    //         color,
    //         obj.position(),
    //         Vec4(0, 0, 0, 0),
    //         [Vec3(0, 1, 0).rotate(rotation), Vec3(0, 3, 0).rotate(rotation)],
    //         obj
    //     )
    // }

    static function ResetTimedVars() {
        HE1JumpCollisionHelper.ms_IsOutOfKeepVelocity <- 0
        HE1JumpCollisionHelper.ms_GravityTimer <- 0
        HE1JumpCollisionHelper.ms_TotalTimer <- 0
        HE1JumpCollisionHelper.ms_KeepVelocityDistance <- 0
        HE1JumpCollisionHelper.ms_KeepVelocityDistance <- 0
    }
    static function CalculateTrajectoryLine(
        obj,
        debugVisual,
        deltaTime,
        isSpring
    ) {
        local position = obj.position()
        local rotation = obj.rotation()
        local color = Vec4(0, 1, 1, 0.7)

        local first_speed = obj.parameters().GetParameterFloat("FirstSpeed")
        local keep_distance = obj
            .parameters()
            .GetParameterFloat("KeepVelocityDistance")
        local spring_dir = Vec3(0, 1, 0)
        local direction = spring_dir.rotate(rotation)

        local positions = []

        local sample_count = 20
        local time_step = 5.0 / sample_count
        for (local i = 0; i < sample_count; i++) {
            local sample_time = i * time_step
            local sample_gravity_time = 0.0
            local sample_keep_distance = 0.0
            local sample_ms_IsOutOfKeepVelocity = false

            if (first_speed * sample_time > keep_distance) {
                sample_keep_distance = keep_distance
                sample_gravity_time = sample_time - keep_distance / first_speed
                sample_ms_IsOutOfKeepVelocity = true
            } else {
                sample_keep_distance = first_speed * sample_time
            }

            local delta_y = 0.0
            if (sample_ms_IsOutOfKeepVelocity) {
                local gravity = 35
                delta_y =
                    first_speed * sample_gravity_time * direction.y() +
                    0.5 * -gravity * (sample_gravity_time * sample_gravity_time)
            }

            local new_pos_x = first_speed * sample_time * direction.x()
            local new_pos_y = delta_y + sample_keep_distance * direction.y()
            local new_pos_z = first_speed * sample_time * direction.z()

            local sample_position = Vec3Add(
                position,
                Vec3(new_pos_x, new_pos_y, new_pos_z)
            )
            positions.append(sample_position)
        }

        for (local i = 0; i < positions.len() - 1; i++) {
            local sample_time = i * time_step
            local is_gravity_phase = first_speed * sample_time > keep_distance
            local segment_color = is_gravity_phase
                ? Vec4(1, 1, 1, 1)
                : Vec4(0, 1, 1, 1)
            debugVisual.DrawLine(
                segment_color,
                Vec3(0, 0, 0),
                Vec4(0, 0, 0, 0),
                [positions[i], positions[i + 1]],
                obj
            )
        }
    }
    static function CalculateTrajectorySphere(
        obj,
        debugVisual,
        deltaTime,
        isSpring
    ) {
        if (HE1JumpCollisionHelper.ms_TotalTimer >= 5) {
            HE1JumpCollisionHelper.ResetTimedVars()
        }
        local position = obj.position()
        local rotation = obj.rotation()
        local color = Vec4(1, 0, 0, 0.5)
        local scale = Vec3(10, 10, 10)

        local first_speed = obj.parameters().GetParameterFloat("FirstSpeed")
        local keep_distance = obj
            .parameters()
            .GetParameterFloat("KeepVelocityDistance")
        local spring_dir = Vec3(0, 1, 0)

        local direction = spring_dir.rotate(rotation)

        local delta_y = HE1JumpCollisionHelper.GetTrajectoryGravity(
            first_speed,
            keep_distance,
            direction.y(),
            HE1JumpCollisionHelper.ms_TotalTimer
        )

        if (HE1JumpCollisionHelper.ms_IsOutOfKeepVelocity)
            HE1JumpCollisionHelper.ms_GravityTimer <-
                HE1JumpCollisionHelper.ms_GravityTimer + deltaTime

        HE1JumpCollisionHelper.ms_TotalTimer <-
            HE1JumpCollisionHelper.ms_TotalTimer + deltaTime

        local new_pos_x = 0.0
        local new_pos_y = 0.0
        local new_pos_z = 0.0

        if (isSpring) {
            new_pos_x =
                first_speed *
                HE1JumpCollisionHelper.ms_TotalTimer *
                direction.x()
            new_pos_y =
                delta_y +
                HE1JumpCollisionHelper.ms_KeepVelocityDistance * direction.y()
            new_pos_z =
                first_speed *
                HE1JumpCollisionHelper.ms_TotalTimer *
                direction.z()
        } else {
            new_pos_x = 0
            new_pos_y = delta_y + ms_KeepVelocityDistance
            new_pos_z = 0
        }

        local new_position = obj.position()
        local position_add = Vec3(new_pos_x, new_pos_y, new_pos_z)
        new_position = Vec3Add(new_position, position_add)
        debugVisual.DrawSphere(color, new_position, 0.4, obj)
    }
    static function Draw(obj, debugVisual, deltaTime, isSpring) {
        HE1JumpCollisionHelper.CalculateTrajectorySphere(
            obj,
            debugVisual,
            deltaTime,
            isSpring
        )
        //HE1JumpCollisionHelper.CalculateTrajectorySphere(obj, debugVisual, deltaTime / 2);
        HE1JumpCollisionHelper.CalculateTrajectoryLine(
            obj,
            debugVisual,
            deltaTime,
            isSpring
        )
    }
}
class Spring {
    static function DynamicDebugVisualEnd(obj) {
        HE1JumpCollisionHelper.ResetTimedVars()
    }
    static function AddDynamicDebugVisual(obj, debugVisual, deltaTime) 
    {
        HE1JumpCollisionHelper.Draw(obj, debugVisual, deltaTime, true)
        //debugVisual.DrawCube(color, position, rotation, scale, obj);
    }
}
function Vec3Multiply(vecOne, vecTwo) {
    return Vec3(
        vecOne.x() * vecTwo.x(),
        vecOne.y() * vecTwo.y(),
        vecOne.z() * vecTwo.z()
    )
}
function Vec3Add(v1, v2) {
    return Vec3(v1.x() + v2.x(), v1.y() + v2.y(), v1.z() + v2.z())
}
