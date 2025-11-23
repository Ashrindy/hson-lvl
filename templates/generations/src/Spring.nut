class Spring {
    static m_total_time = 0
    static keep_velocity_distance = 0
    static m_gravity_time = 0
    static act_gravity = false
    static dolog = true

    static function getTrajectoryGravity(
        first_speed,
        ignore_distance,
        y_direction,
        total_time
    ) {
        local delta_y = 0.0
        local gravity = 35

        if (first_speed * total_time > ignore_distance && !Spring.act_gravity) {
            local temp_distance = first_speed * total_time
            Spring.keep_velocity_distance <- temp_distance
            Spring.m_gravity_time <- 0.0
            Spring.act_gravity <- true
        } else {
            if (!Spring.act_gravity) {
                local temp_distance = first_speed * total_time
                Spring.keep_velocity_distance <- temp_distance
            }
        }

        if (Spring.act_gravity) {
            local gravity_time = Spring.m_gravity_time
            delta_y =
                first_speed * gravity_time * y_direction +
                0.5 * -gravity * (gravity_time * gravity_time)
        }

        return delta_y
    }
    static function AddDebugVisual(obj, debugVisual) 
    {
        local position = obj.position()
        local rotation = obj.rotation()
        local color = Vec4(1, 0, 1, 1)
        local scale = Vec3(10, 10, 10)
        debugVisual.DrawLine(color, obj.position(), Vec4(0,0,0,0), [Vec3(0,1,0).rotate(rotation), Vec3(0,3,0).rotate(rotation)], obj)
    }
     static function DynamicDebugVisualEnd(obj) {
        Spring.m_gravity_time <- 0;
        Spring.m_total_time <- 0;
        Spring.keep_velocity_distance <- 0;
     }
    static function AddDynamicDebugVisual(obj, debugVisual, deltaTime) {
        
        local position = obj.position()
        local rotation = obj.rotation()
        local color = Vec4(1, 0, 1, 1)
        local scale = Vec3(10, 10, 10)

        local first_speed = obj.parameters().GetParameterFloat("FirstSpeed")
        local keep_distance = obj
            .parameters()
            .GetParameterFloat("KeepVelocityDistance")
        local spring_dir = Vec3(0, 1, 0)

        local direction = spring_dir.rotate(rotation)

        local delta_y = Spring.getTrajectoryGravity(
            first_speed,
            keep_distance,
            direction.y(),
            Spring.m_total_time
        )

        local current_time = Spring.m_total_time
        if (current_time >= 5) {
            current_time = 0.0 // Reset at 5 seconds
            Spring.m_gravity_time <- 0
        }
        current_time += deltaTime
        Spring.m_total_time <- current_time

        if (Spring.act_gravity)
            Spring.m_gravity_time <- Spring.m_gravity_time + deltaTime

        local new_pos_x = 0.0
        local new_pos_y = 0.0
        local new_pos_z = 0.0

        // if (m_mode == SPRING)
        // {
        
        new_pos_x = first_speed * Spring.m_total_time * direction.x()
        new_pos_y = delta_y + Spring.keep_velocity_distance * direction.y()
        new_pos_z = first_speed * Spring.m_total_time * direction.z()
        // }
        // else
        // {
        //     new_pos_x = 0;
        //     new_pos_y = delta_y + keep_velocity_distance;
        //     new_pos_z = 0;
        // }

        local new_position = obj.position()
        local position_add = Vec3(new_pos_x, new_pos_y, new_pos_z)
        new_position = Vec3Add(new_position, position_add)
        debugVisual.DrawCube(color, new_position, rotation, scale, obj)

        // 
        if (Spring.dolog) 
        {
            print("NOT RELOADED");
            print(
                "direction: " +
                    direction.x() +
                    " " +
                    direction.y() +
                    " " +
                    direction.z() +
                    "\n"
            )

            print(
                "new_position: " +
                    new_position.x() +
                    " " +
                    new_position.y() +
                    " " +
                    new_position.z() +
                    "\n"
            )

            print("TotalTime " + Spring.m_total_time + "\n")
            print("first_speed " + first_speed + "\n")
            print("keep_distance " + keep_distance + "\n")
        }

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
