//
// Walker.cpp
//
// Clark Kromenaker
//
#include "Walker.h"

#include "Actor.h"
#include "Animator.h"
#include "CharacterManager.h"
#include "Debug.h"
#include "GEngine.h"
#include "GKActor.h"
#include "Heading.h"
#include "MeshRenderer.h"
#include "Quaternion.h"
#include "Scene.h"
#include "Services.h"
#include "StringUtil.h"
#include "Vector3.h"
#include "WalkerBoundary.h"

TYPE_DEF_CHILD(Component, Walker);

Walker::Walker(Actor* owner) : Component(owner),
    mGKOwner(static_cast<GKActor*>(owner))
{
    
}

void Walker::WalkTo(const Vector3& position, WalkerBoundary* walkerBoundary, std::function<void()> finishCallback)
{
    WalkTo(position, Heading::None, walkerBoundary, finishCallback);
}

void Walker::WalkTo(const Vector3& position, const Heading& heading, WalkerBoundary* walkerBoundary, std::function<void()> finishCallback)
{
    // Save walker boundary.
    mWalkerBoundary = walkerBoundary;
    
    // Save finish callback.
    mFinishedPathCallback = finishCallback;
    
    // It's possible for a "walk to" to be received in the middle of another walk sequence.
    // In that case, the current walk sequence is canceled and a new one is constructed.
    bool wasMidWalk = IsMidWalk();
    
    // Ok, starting a new walk plan.
    mWalkActions.clear();
    
    // If heading is specified, save "turn to face" action.
    if(heading.IsValid())
    {
        WalkAction turnAction;
        turnAction.op = WalkOp::TurnToFace;
        turnAction.facingDir = heading.ToVector();
        mWalkActions.push_back(turnAction);
    }
    
    // Do we need to walk?
    if((GetOwner()->GetPosition() - position).GetLengthSq() > kAtNodeDistSq)
    {
        // We will walk, so save the "walk end" action.
        WalkAction endWalkAction;
        endWalkAction.op = WalkOp::FollowPathEnd;
        mWalkActions.push_back(endWalkAction);
        
        // Calculate a path.
        CalculatePath(GetOwner()->GetPosition(), position);
        
        // Follow the path.
        WalkAction followPathAction;
        followPathAction.op = WalkOp::FollowPath;
        mWalkActions.push_back(followPathAction);
        
        // Determine start walk action. This depends on whether the next node is right in front of us, behind, etc.
        // If first path node is in front of the walker, no need to play turn anims.
        Vector3 toNext = (mPath.front() - GetOwner()->GetPosition()).Normalize();
        if(Vector3::Dot(GetOwner()->GetForward(), toNext) > -0.5f)
        {
            // Add start walk action, but ONLY IF wasn't previously mid-walk.
            // If was already walking and next node is in same direction...just keep walking!
            if(!wasMidWalk)
            {
                WalkAction startWalkAction;
                startWalkAction.op = WalkOp::FollowPathStart;
                mWalkActions.push_back(startWalkAction);
            }
        }
        else
        {
            // First node IS NOT in front of walker, so gotta turn using fancy anims.
            
            // So, we need to play start walk anim while also turning to face next node.
            WalkAction startWalkTurnAction;
            startWalkTurnAction.facingDir = toNext;
            
            // For initial direction to turn, let's use the goal node direction.
            // Just thinking about the real world...you usually turn towards your goal, right?
            Vector3 toLastDir = (mPath.front() - GetOwner()->GetPosition()).Normalize();
            Vector3 cross = Vector3::Cross(GetOwner()->GetForward(), toLastDir);
            if(cross.y > 0)
            {
                startWalkTurnAction.op = WalkOp::FollowPathStartTurnRight;
            }
            else
            {
                startWalkTurnAction.op = WalkOp::FollowPathStartTurnLeft;
            }
            
            mWalkActions.push_back(startWalkTurnAction);
        }
    }
    
    // OK, walk sequence has been created - let's start doing it!
    //std::cout << "Walk Sequence Begin!" << std::endl;
    NextAction();
}

void Walker::WalkToSee(const std::string& targetName, const Vector3& targetPosition, WalkerBoundary* walkerBoundary, std::function<void()> finishCallback)
{
    mWalkToSeeTarget = targetName;
    mWalkToSeeTargetPosition = targetPosition;
    
    // Check whether thing is already in view.
    // If so, we don't even need to walk (but may need to turn-to-face).
    Vector3 facingDir;
    if(IsWalkToSeeTargetInView(facingDir))
    {
        // Be sure to save finish callback in this case - it usually happens in walk to.
        mFinishedPathCallback = finishCallback;
        
        // Clear current walk.
        bool wasMidWalk = IsMidWalk();
        mWalkActions.clear();
        
        // If mid-walk, we need to stop walking before turning.
        if(wasMidWalk)
        {
            WalkAction stopWalkAction;
            stopWalkAction.op = WalkOp::FollowPathEnd;
            mWalkActions.push_back(stopWalkAction);
        }
        
        // No need to walk, but do turn to face the thing.
        WalkAction turnAction;
        turnAction.op = WalkOp::TurnToFace;
        turnAction.facingDir = facingDir;
        mWalkActions.push_back(turnAction);
        
        // OK, walk sequence has been created - let's start doing it!
        NextAction();
    }
    else
    {
        // Specify a "dummy" heading here so that the "turn to face" action is put into the walk plan.
        // Later on, when the object comes into view, we'll replace this with the actual direction to turn.
        WalkTo(targetPosition, Heading::FromDegrees(0.0f), walkerBoundary, finishCallback);
    }
}

void Walker::OnUpdate(float deltaTime)
{
    // Debug draw the path.
    if(mPath.size() > 0)
    {
        Vector3 prev = mPath.back();
        for(int i = static_cast<int>(mPath.size()) - 2; i >= 0; i--)
        {
            Debug::DrawLine(prev, mPath[i], Color32::Orange);
            prev = mPath[i];
        }
    }
    
    // Process outstanding walk actions.
    if(mWalkActions.size() > 0)
    {
        // Increase current action timer.
        mCurrentWalkActionTimer += deltaTime;
        
        WalkAction& currentAction = mWalkActions.back();
        if(currentAction.op == WalkOp::FollowPathStart)
        {
            // Possible to finish path while still in the start phase, for very short paths.
            if(AdvancePath())
            {
                // In this case, we want to skip all follow path bits and go right to "follow path end".
                while(mWalkActions.size() > 0 && mWalkActions.back().op != WalkOp::FollowPathEnd)
                {
                    mWalkActions.pop_back();
                }
                NextAction();
            }
            else
            {
                Vector3 toNextDir = (mPath.back() - GetOwner()->GetPosition()).Normalize();
                TurnToFace(deltaTime, GetOwner()->GetForward(), toNextDir, kWalkTurnSpeed);
            }
        }
        else if(currentAction.op == WalkOp::FollowPathStartTurnLeft || currentAction.op == WalkOp::FollowPathStartTurnRight)
        {
            if(!AdvancePath())
            {
                // HACK: Wait until the "turn" anim has the character put its foot down before starting to turn to the target.
                // About 0.66s seems to look OK, but could probably be finessed a bit.
                if(mCurrentWalkActionTimer > 0.66f)
                {
                    Vector3 toNextDir = (mPath.back() - GetOwner()->GetPosition()).Normalize();
                    TurnToFace(deltaTime, GetOwner()->GetForward(), toNextDir, kWalkTurnSpeed);
                }
            }
        }
        else if(currentAction.op == WalkOp::FollowPath)
        {
            // If we have a "walk to see" target, check whether it has come into view.
            Vector3 facingDir;
            if(IsWalkToSeeTargetInView(facingDir))
            {
                // Make sure final action is a "turn to face" using this facing dir.
                assert(mWalkActions[0].op == WalkOp::TurnToFace);
                mWalkActions[0].facingDir = facingDir;
                
                PopAndNextAction();
            }
            // Otherwise, see if finished following path.
            else if(AdvancePath())
            {
                // Path is finished - move on to next step in sequence.
                PopAndNextAction();
            }
            // Otherwise, just keep on following that path!
            else
            {
                // Still following path - turn to face next node in path.
                Vector3 toNextDir = (mPath.back() - GetOwner()->GetPosition()).Normalize();
                TurnToFace(deltaTime, GetOwner()->GetForward(), toNextDir, kWalkTurnSpeed);
            }
        }
        else if(currentAction.op == WalkOp::TurnToFace)
        {
            if(TurnToFace(deltaTime, GetOwner()->GetForward(), currentAction.facingDir, kTurnSpeed))
            {
                // Done turning to face, do next action in sequence.
                PopAndNextAction();
            }
        }
    }
}

void Walker::PopAndNextAction()
{
    // Pop action from sequence.
    if(mWalkActions.size() > 0)
    {
        mPrevWalkOp = mWalkActions.back().op;
        mWalkActions.pop_back();
    }
    
    // Go to next action.
    NextAction();
}

void Walker::NextAction()
{
    mCurrentWalkActionTimer = 0.0f;
    
    // Do next action in sequence, if any.
    if(mWalkActions.size() > 0)
    {
        if(mWalkActions.back().op == WalkOp::FollowPathStart)
        {
            std::cout << "Follow Path Start" << std::endl;
            
            AnimParams animParams;
            animParams.animation = mCharConfig->walkStartAnim;
            animParams.allowMove = true;
            animParams.finishCallback = std::bind(&Walker::PopAndNextAction, this);
            GEngine::Instance()->GetScene()->GetAnimator()->Start(animParams);
        }
        else if(mWalkActions.back().op == WalkOp::FollowPathStartTurnLeft)
        {
            std::cout << "Follow Path Start (Turn Left)" << std::endl;
            
            AnimParams animParams;
            animParams.animation = mCharConfig->walkStartTurnLeftAnim;
            animParams.allowMove = true;
            animParams.finishCallback = std::bind(&Walker::PopAndNextAction, this);
            GEngine::Instance()->GetScene()->GetAnimator()->Start(animParams);
        }
        else if(mWalkActions.back().op == WalkOp::FollowPathStartTurnRight)
        {
            std::cout << "Follow Path Start (Turn Right)" << std::endl;
            
            AnimParams animParams;
            animParams.animation = mCharConfig->walkStartTurnRightAnim;
            animParams.allowMove = true;
            animParams.finishCallback = std::bind(&Walker::PopAndNextAction, this);
            GEngine::Instance()->GetScene()->GetAnimator()->Start(animParams);
        }
        else if(mWalkActions.back().op == WalkOp::FollowPath)
        {
            std::cout << "Follow Path" << std::endl;
            
            AnimParams animParams;
            animParams.animation = mCharConfig->walkLoopAnim;
            animParams.allowMove = true;
            animParams.finishCallback = std::bind(&Walker::ContinueWalk, this);
            
            if(mPrevWalkOp == WalkOp::FollowPathStartTurnRight)
            {
                animParams.startFrame = 10;
            }
            
            GEngine::Instance()->GetScene()->GetAnimator()->Start(animParams);
        }
        else if(mWalkActions.back().op == WalkOp::FollowPathEnd)
        {
            std::cout << "Follow Path End" << std::endl;
            GEngine::Instance()->GetScene()->GetAnimator()->Stop(mCharConfig->walkStartAnim);
            GEngine::Instance()->GetScene()->GetAnimator()->Stop(mCharConfig->walkLoopAnim);
            
            AnimParams animParams;
            animParams.animation = mCharConfig->walkStopAnim;
            animParams.allowMove = true;
            animParams.finishCallback = std::bind(&Walker::PopAndNextAction, this);
            GEngine::Instance()->GetScene()->GetAnimator()->Start(animParams);
        }
        else if(mWalkActions.back().op == WalkOp::TurnToFace)
        {
            std::cout << "Turn To Face" << std::endl;
            // Handled in Update
        }
    }
    else
    {
        std::cout << "Walk Sequence Done!" << std::endl;
        
        // No actions left in sequence => walk is finished.
        OnWalkToFinished();
    }
}

void Walker::ContinueWalk()
{
    // This function is just a helper to loop the "walk loop" anim.
    // Just make sure the current action is still "follow path" and if so we can play the loop anim one more time.
    if(mWalkActions.size() > 0 && mWalkActions.back().op == WalkOp::FollowPath)
    {
        AnimParams animParams;
        animParams.animation = mCharConfig->walkLoopAnim;
        animParams.allowMove = true;
        animParams.finishCallback = std::bind(&Walker::ContinueWalk, this);
        GEngine::Instance()->GetScene()->GetAnimator()->Start(animParams);
    }
}

void Walker::OnWalkToFinished()
{
    // Make sure state variables are cleared.
    mPath.clear();
    mWalkToSeeTarget.clear();
    
    // Call finished callback.
    if(mFinishedPathCallback != nullptr)
    {
        mFinishedPathCallback();
        mFinishedPathCallback = nullptr;
    }
}

bool Walker::IsWalkToSeeTargetInView(Vector3& outTurnToFaceDir)
{
    // Not in view if it doesn't exist!
    if(mWalkToSeeTarget.empty()) { return false; }
    
    // Create ray from head in direction of target position.
    Vector3 headPos = mGKOwner->GetHeadPosition();
    Vector3 dir = (mWalkToSeeTargetPosition - headPos).Normalize();
    Ray ray(headPos, dir);
    
    // If hit the target with the ray, it must be in view.
    SceneCastResult result = GEngine::Instance()->GetScene()->Raycast(ray, false, mGKOwner);
    if(StringUtil::EqualsIgnoreCase(result.hitInfo.name, mWalkToSeeTarget))
    {
        // Convert ray direction to a "facing" direction,
        dir.y = 0.0f;
        outTurnToFaceDir = dir.Normalize();
        return true;
    }
    return false;
}

bool Walker::CalculatePath(const Vector3& startPos, const Vector3& endPos)
{
    if(mWalkerBoundary == nullptr) { return false; }
    
    if(mWalkerBoundary->FindPath(startPos, endPos, mPath))
    {
        // Try to reduce the number of nodes in the path.
        // The walker nodes are very close to one another.
        int iterations = 2;
        while(iterations > 0)
        {
            bool getRid = false;
            for(auto it = mPath.end() - 1; it != mPath.begin(); --it)
            {
                if(getRid)
                {
                    it = mPath.erase(it);
                }
                getRid = !getRid;
            }
            --iterations;
        }
        
        // The path generated by A* is likely too "fidgety" for human-like movement.
        // Clean up path by getting rid of excess nodes.
        /*
        for(auto it = mPath.end() - 1; it != mPath.begin(); --it)
        {
            if(it + 1 == mPath.end() || it == mPath.begin()) { continue; }
            
            Vector3 pointBefore = *(it - 1);
            Vector3 point = *(it);
            Vector3 pointAfter = *(it + 1);
            
            Vector3 toAfter = Vector3::Normalize(pointAfter - point);
            Vector3 toBefore = Vector3::Normalize(pointBefore - point);
            
            float dot = Vector3::Dot(toAfter, toBefore);
            if(dot < -0.65f)
            {
                mPath.erase(it);
            }
        }
        */
    }
    else
    {
        // Couldn't find a path so...let's just walk straight to the spot!
        // This'll probably look broken...but it's the ultimate fallback.
        mPath.clear();
        mPath.push_back(endPos);
        mPath.push_back(startPos);
    }
    return true;
}

bool Walker::AdvancePath()
{
    if(mPath.size() > 0)
    {
        Debug::DrawLine(GetOwner()->GetPosition(), mPath.back(), Color32::White);
        
        // When walking, we're always mainly interested in getting to the end of the path.
        // Particularly, don't ONLY check distance to last node!
        // If we skipped a node (maybe overshot it), that's OK!
        int atNode = -1;
        for(int i = 0; i < mPath.size(); i++)
        {
            Vector3 toNode = mPath[i] - mGKOwner->GetPosition();
            if(toNode.GetLengthSq() < kAtNodeDistSq)
            {
                atNode = i;
                break;
            }
        }
        if(atNode > -1)
        {
            while(mPath.size() > atNode)
            {
                mPath.pop_back();
            }
        }
    }
    
    // Return whether path is completed.
    return mPath.size() == 0;
}

bool Walker::TurnToFace(float deltaTime, const Vector3& currentDir, const Vector3& desiredDir, float turnSpeed, int turnDir, bool aboutOwnerPos)
{
    bool doneTurning = false;
    
    // What angle do I need to rotate to face the desired direction?
    float currToDesiredAngle = Math::Acos(Vector3::Dot(currentDir, desiredDir));
    if(currToDesiredAngle > kAtHeadingRadians)
    {
        float rotateDirection = turnDir;
        if(rotateDirection == 0)
        {
            // Which way do I rotate to get to facing direction I want?
            // Can use y-axis of cross product to determine this.
            Vector3 cross = Vector3::Cross(currentDir, desiredDir);
            
            // If y-axis is zero, it means vectors are parallel (either exactly facing or exactly NOT facing).
            // In that case, 1.0f default is fine. Otherwise, we want either 1.0f or -1.0f.
            rotateDirection = cross.y >= 0.0f ? 1 : -1;
        }
        
        // Determine how much we'll rotate this frame.
        // If it's enough to where our angle will be less than the "at heading" rotation, just set heading exactly.
        float thisFrameRotateAngle = turnSpeed * deltaTime;
        if(currToDesiredAngle - thisFrameRotateAngle <= kAtHeadingRadians)
        {
            thisFrameRotateAngle = currToDesiredAngle;
            doneTurning = true;
        }
        
        // Factor direction into angle.
        thisFrameRotateAngle *= rotateDirection;
        
        // Rotate actor's mesh to face the desired direction, rotating around the actor's position.
        // Remember, the GKActor follows the mesh's position during animations (based on hip bone placement). But the mesh itself may have a vastly different origin b/c of playing animation.
        Transform* meshTransform = mGKOwner->GetMeshRenderer()->GetOwner()->GetTransform();
        if(aboutOwnerPos)
        {
            meshTransform->RotateAround(mGKOwner->GetPosition(), Vector3::UnitY, thisFrameRotateAngle);
        }
        else
        {
            meshTransform->Rotate(Vector3::UnitY, thisFrameRotateAngle);
        }
    }
    else
    {
        doneTurning = true;
    }
    return doneTurning;
}
