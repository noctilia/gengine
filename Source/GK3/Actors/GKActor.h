//
// Clark Kromenaker
//
// A GK "actor" is a humanoid character or animal.
// Actors can walk around, animate, perform facial expressions and eye contact, etc.
//
#pragma once
#include "GKObject.h"

#include <string>

#include "Heading.h"
#include "Walker.h"

struct CharacterConfig;
class FaceController;
class GAS;
class GasPlayer;
class Model;
struct SceneActor;
class SceneData;
class VertexAnimation;
class VertexAnimator;
struct VertexAnimParams;

class GKActor : public GKObject
{
public:
    GKActor();
	GKActor(Model* model);
    GKActor(const SceneActor& actorDef);

    void Init(const SceneData& sceneData);
    
    enum class FidgetType
    {
        None,
        Idle,
        Talk,
        Listen
    };
	void StartFidget(FidgetType type);
    void StopFidget(std::function<void()> callback = nullptr);

    void SetIdleFidget(GAS* fidget);
    void SetTalkFidget(GAS* fidget);
    void SetListenFidget(GAS* fidget);
    GAS* GetIdleFidget() { return mIdleFidget; }
    GAS* GetTalkFidget() { return mTalkFidget; }
    GAS* GetListenFidget() { return mListenFidget; }

	void TurnTo(const Heading& heading, std::function<void()> finishCallback);
	void WalkTo(const Vector3& position, const Heading& heading, std::function<void()> finishCallback);
	void WalkTo(const Vector3& position, std::function<void()> finishCallback);
	void WalkToAnimationStart(Animation* anim, std::function<void()> finishCallback);
	void WalkToSee(const std::string& targetName, const Vector3& targetPosition, std::function<void()> finishCallback);

    Vector3 GetWalkDestination() const;
    bool AtPosition(const Vector3& position) { return mWalker->AtPosition(position); }
    bool IsWalking() const { return mWalker->IsWalking(); }
    void SkipWalk() { mWalker->SkipToEnd(); }
    void SnapToFloor();
    
	FaceController* GetFaceController() const { return mFaceController; }
	Vector3 GetHeadPosition() const;
    
    void SetPosition(const Vector3& position);
    void Rotate(float rotationAngle);
    void SetHeading(const Heading& heading) override;

    void StartAnimation(VertexAnimParams& animParams) override;
    void SampleAnimation(VertexAnimParams& animParams, int frame) override;
    void StopAnimation(VertexAnimation* anim = nullptr) override;
    MeshRenderer* GetMeshRenderer() const override { return mMeshRenderer; }
	
protected:
    void OnActive() override;
    void OnInactive() override;
	void OnUpdate(float deltaTime) override;
    
private:
	// The character's configuration, which defines helpful parameters for controlling the actor.
	const CharacterConfig* mCharConfig = nullptr;

    // The fidget the actor is currently playing.
    FidgetType mActiveFidget = FidgetType::None;
	
	// The actor's walking control.
	Walker* mWalker = nullptr;
	
	// The actor's face control.
	FaceController* mFaceController = nullptr;
    
    // GAS scripts to use when actor is idle, talking, or listening.
    GAS* mIdleFidget = nullptr;
    GAS* mTalkFidget = nullptr;
    GAS* mListenFidget = nullptr;

    // The actor/mesh renderer used to render this object's model.
    // For props, model actor == this. But for characters, model actor is a separate actor.
    Actor* mModelActor = nullptr;
    MeshRenderer* mMeshRenderer = nullptr;

    // Many objects animate using vertex animations.
    VertexAnimator* mVertexAnimator = nullptr;

    // GAS player allows object to animate in an automated/scripted fashion based on some simple command statements.
    GasPlayer* mGasPlayer = nullptr;
    
    // Vertex anims often change the position of the mesh, but that doesn't mean the actor's position should change.
    // Sometimes we allow a vertex anim to affect the actor's position.
    bool mVertexAnimAllowMove = false;
    
    // For non-move vertex anims, the actor resets to its original position/rotation when the anim stops.
    // So, we must save the start position/rotation for that purpose.
    Vector3 mStartVertexAnimPosition;
    Quaternion mStartVertexAnimRotation;
    
    // Usually, the model drives the actor's position (think: root motion).
    // To do that, we track the models last position/rotation and move the actor every frame to keep up.
    Vector3 mLastModelPosition;
    Quaternion mLastModelRotation;

    void OnVertexAnimationStop();
    
    Vector3 GetModelPosition();
    Quaternion GetModelRotation();
    
    void SyncModelToActor();
    void SyncActorToModel();

    GAS* GetGasForFidget(FidgetType type);
    void CheckUpdateActiveFidget(FidgetType changedType);
};
