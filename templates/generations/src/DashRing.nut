class DashRing 
{
    static function DynamicDebugVisualEnd(obj) 
    {
        HE1JumpCollisionHelper.ResetTimedVars()
    }
    static function AddDynamicDebugVisual(obj, debugVisual, deltaTime) 
    {
        HE1JumpCollisionHelper.Draw(obj, debugVisual, deltaTime, true)
    }
}