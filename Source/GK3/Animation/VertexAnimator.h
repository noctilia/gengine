//
// VertexAnimator.h
//
// Clark Kromenaker
//
// A component that can play vertex animations on an actor.
//
#pragma once
#include "Component.h"

#include <functional>

#include "Heading.h"
#include "Vector3.h"

class MeshRenderer;
class VertexAnimation;

struct VertexAnimParams
{
	// The anim to play.
	VertexAnimation* vertexAnimation = nullptr;
	
	// Rate to play the animation at.
	int framesPerSecond = 15;
	
	// Time to start the animation at.
	float startTime = 0.0f;
    //TODO: startFrame?
    
    // An absolute anim plays from a specific position/rotation
    // (in contrast to a "relative" anim that plays from model's last position/rotation).
    bool absolute = false;
    Vector3 absolutePosition;
    Heading absoluteHeading = Heading::None;
    
    // If true, animation can move associated character (kind of like "root motion").
    bool allowMove = false;
    
    // If true, this anim was started from an autoscript (GAS).
    bool fromAutoScript = false;
    
	// A callback to fire on animation stop.
	std::function<void()> stopCallback = nullptr;
};

class VertexAnimator : public Component
{
	TYPE_DECL_CHILD();
public:
	VertexAnimator(Actor* owner);
	
    void Start(const VertexAnimParams& params);
	void Stop(VertexAnimation* anim = nullptr);
	
	void Sample(VertexAnimation* animation, int frame);
	
	bool IsPlaying() const { return mVertexAnimation != nullptr; }
	
protected:
	void OnUpdate(float deltaTime) override;
	
private:
	// The mesh renderer that will be animated.
	MeshRenderer* mMeshRenderer = nullptr;
	
	// How many frames per second to run at. Default is 15 (from GK3 docs).
	int mFramesPerSecond = 15;
	
	// A currently running vertex animation, if any.
	VertexAnimation* mVertexAnimation = nullptr;
	
	// Callback that is fired when the animation stops.
	// "Stops" means manually stopped OR reached end of playback!
	std::function<void()> mStopCallback = nullptr;
	
	// Timer for tracking progress on vertex animation.
	float mAnimationTimer = 0.0f;
	
    void TakeSample(VertexAnimation* animation, int frame);
	void TakeSample(VertexAnimation* animation, float time);
};
