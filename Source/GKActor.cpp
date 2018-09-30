//
// GKActor.cpp
//
// Clark Kromenaker
//
#include "GKActor.h"

#include "AnimationPlayer.h"
#include "GasPlayer.h"
#include "GEngine.h"
#include "MeshRenderer.h"
#include "Scene.h"

GKActor::GKActor() : Actor()
{
    // Usually, we'll have a mesh to display.
    mMeshRenderer = AddComponent<MeshRenderer>();
    
    // Create animation player.
    AnimationPlayer* animationPlayer = AddComponent<AnimationPlayer>();
    animationPlayer->SetMeshRenderer(mMeshRenderer);
    
    // GasPlayer is used to play gas script files.
    // Actor also needs a way to play animations, and the GasPlayer needs to know about it.
    mGasPlayer = AddComponent<GasPlayer>();
    mGasPlayer->SetAnimationPlayer(animationPlayer);
}

void GKActor::Update(float deltaTime)
{
	// Stay attached to the floor, usually.
	Vector3 position = GetPosition();
	position.SetY(GEngine::inst->GetScene()->GetFloorY(position));
	SetPosition(position);
}

void GKActor::SetState(GKActor::State state)
{
    // Save state.
    mState = state;
    
    // Set appropriate Gas to play.
    switch(mState)
    {
        case State::Idle:
            mGasPlayer->SetGas(mIdleGas);
            break;
            
        case State::Talk:
            mGasPlayer->SetGas(mTalkGas);
            break;
            
        case State::Listen:
            mGasPlayer->SetGas(mListenGas);
            break;
    }
}
