
class SpringClassic {

    static m_total_time = 0;
    static keep_velocity_distance = 0;
    static m_gravity_time = 0;
    static act_gravity = false;
    static function getTrajectoryGravity(first_speed, ignore_distance, y_direction, total_time)

{
    local delta_y = 0.0;
    local gravity = 35;
    
    if ((first_speed * total_time) > ignore_distance && !SpringClassic.act_gravity)
{
    local temp_distance = first_speed * total_time;
    SpringClassic.keep_velocity_distance <- temp_distance;
    SpringClassic.m_gravity_time <- 0.0;
    SpringClassic.act_gravity <- true;
}
else
{
    if (!SpringClassic.act_gravity)
    {
        local temp_distance = first_speed * total_time;
        SpringClassic.keep_velocity_distance <- temp_distance;
    }
}

if (SpringClassic.act_gravity)
{
    local gravity_time = SpringClassic.m_gravity_time;
    delta_y = (first_speed * gravity_time * y_direction)
              + (0.5 * -gravity * (gravity_time * gravity_time));
}
    
    return delta_y;
}
    static function AddDynamicDebugVisual(obj, debugVisual, deltaTime)  
    {
        print("AA111");
        local position = obj.position();
        local rotation = obj.rotation();
        local color = Vec4(1, 0, 1, 1);
local scale = Vec3(10, 10, 10);
        

        local first_speed  = obj.parameters().GetParameterFloat("FirstSpeed");
        local keep_distance = obj.parameters().GetParameterFloat("KeepVelocityDistance");
        local spring_dir = Vec3(0,0,1);
        
        local direction = spring_dir.rotate(rotation);

        local delta_y = SpringClassic.getTrajectoryGravity(first_speed, keep_distance, direction.y(), SpringClassic.m_total_time);

        local current_time = SpringClassic.m_total_time;
if (current_time >= 5)
{
    current_time = 0.0;  // Reset at 5 seconds
SpringClassic.m_gravity_time <- 0;
}
current_time += deltaTime;
SpringClassic.m_total_time <- current_time;

if (SpringClassic.act_gravity)
    SpringClassic.m_gravity_time <- SpringClassic.m_gravity_time +  deltaTime;
print("TotalTime " + SpringClassic.m_total_time + "\n");
print("first_speed " + first_speed + "\n");
print("keep_distance " + keep_distance + "\n");

local new_pos_x = 0.0;
local new_pos_y = 0.0;
local new_pos_z = 0.0;

// if (m_mode == SPRING)
// {
    new_pos_x = (first_speed * SpringClassic.m_total_time) * direction.x();
    new_pos_y = (delta_y + (SpringClassic.keep_velocity_distance * direction.y()));
    new_pos_z = (first_speed * SpringClassic.m_total_time) * direction.z();
// }
// else
// {
//     new_pos_x = 0;
//     new_pos_y = delta_y + keep_velocity_distance;
//     new_pos_z = 0;
// }

local new_position = obj.position();
local position_add = Vec3(new_pos_x, new_pos_y,new_pos_z );
new_position = Vec3Add(new_position, position_add);
debugVisual.DrawCube(color, new_position, rotation, scale, obj);


        print("direction: " + direction.x() + " " + direction.y() + " " + direction.z() + "\n");
        
        print("new_position: " + new_position.x() + " " + new_position.y() + " " + new_position.z() + "\n");
        //debugVisual.DrawCube(color, position, rotation, scale, obj);
    }
}
function Vec3Multiply(vecOne, vecTwo) 
{
    return Vec3(vecOne.x() * vecTwo.x(), vecOne.y() * vecTwo.y(), vecOne.z() * vecTwo.z());
}
function Vec3Add(v1, v2) 
{
    return Vec3(v1.x() + v2.x(), v1.y() + v2.y(),v1.z() + v2.z());
}
function quat_rotate_vec3(q, v)
{
    local qx = q.x(), qy = q.y(), qz = q.z(), qw = q.w();
    local vx = v.x(), vy = v.y(), vz = v.z();

    // t = 2 * cross(q.xyz, v)
    local tx = 2 * (qy * vz - qz * vy);
    local ty = 2 * (qz * vx - qx * vz);
    local tz = 2 * (qx * vy - qy * vx);

    // v' = v + qw * t + cross(q.xyz, t)
    local rx = vx + qw * tx + (qy * tz - qz * ty);
    local ry = vy + qw * ty + (qz * tx - qx * tz);
    local rz = vz + qw * tz + (qx * ty - qy * tx);

    return Vec3(rx, ry, rz);
}
