//
// SheepAPI.cpp
//
// Clark Kromenaker
//
#include "SheepAPI.h"

#include "AnimationPlayer.h"
#include "CharacterManager.h"
#include "GEngine.h"
#include "GKActor.h"
#include "Scene.h"
#include "Services.h"
#include "StringUtil.h"

// Required for macros to work correctly with "string" instead of "std::string".
using namespace std;

// Some defines, for readability below.
#define WAITABLE true
#define IMMEDIATE false

#define DEV_FUNC true
#define REL_FUNC false

std::vector<SysImport> sysFuncs;

void AddSysFuncDecl(const std::string& name, char retType, std::initializer_list<char> argTypes, bool waitable, bool dev)
{
	SysImport sysFunc;
	sysFunc.name = name;
	sysFunc.returnType = retType;
	for(auto argType : argTypes)
	{
		sysFunc.argumentTypes.push_back(argType);
	}
	sysFunc.waitable = waitable;
	sysFunc.devOnly = dev;
	sysFuncs.push_back(sysFunc);
}

// Maps from function name to a function pointer.
// Each map is for a different number of arguments (0-8).
std::map<std::string, Value (*)(const Value&)> map0;
std::map<std::string, Value (*)(const Value&)> map1;
std::map<std::string, Value (*)(const Value&, const Value&)> map2;
std::map<std::string, Value (*)(const Value&, const Value&, const Value&)> map3;
std::map<std::string, Value (*)(const Value&, const Value&, const Value&,
                                const Value&)> map4;
std::map<std::string, Value (*)(const Value&, const Value&, const Value&,
                                const Value&, const Value&)> map5;
std::map<std::string, Value (*)(const Value&, const Value&, const Value&,
                                const Value&, const Value&, const Value&)> map6;
std::map<std::string, Value (*)(const Value&, const Value&, const Value&,
                                const Value&, const Value&, const Value&,
                                const Value&)> map7;
std::map<std::string, Value (*)(const Value&, const Value&, const Value&,
                                const Value&, const Value&, const Value&,
                                const Value&, const Value&)> map8;

Value CallSysFunc(const std::string& name)
{
	auto it = map0.find(name);
	if(it != map0.end())
	{
		return it->second(0);
	}
	else
	{
		std::cout << "Couldn't find SysFunc0 " << name << std::endl;
		return Value(0);
	}
}

Value CallSysFunc(const std::string& name, const Value& x1)
{
	auto it = map1.find(name);
	if(it != map1.end())
	{
		return it->second(x1);
	}
	else
	{
		std::cout << "Couldn't find SysFunc1 " << name << std::endl;
		return Value(0);
	}
}

Value CallSysFunc(const std::string& name, const Value& x1, const Value& x2)
{
	auto it = map2.find(name);
	if(it != map2.end())
	{
		return it->second(x1, x2);
	}
	else
	{
		std::cout << "Couldn't find SysFunc2 " << name << std::endl;
		return Value(0);
	}
}

Value CallSysFunc(const std::string& name, const Value& x1, const Value& x2, const Value& x3)
{
	auto it = map3.find(name);
	if(it != map3.end())
	{
		return it->second(x1, x2, x3);
	}
	else
	{
		std::cout << "Couldn't find SysFunc3 " << name << std::endl;
		return Value(0);
	}
}

Value CallSysFunc(const std::string& name, const Value& x1, const Value& x2, const Value& x3,
                  const Value& x4)
{
	auto it = map4.find(name);
	if(it != map4.end())
	{
		return it->second(x1, x2, x3, x4);
	}
	else
	{
		std::cout << "Couldn't find SysFunc4 " << name << std::endl;
		return Value(0);
	}
}

Value CallSysFunc(const std::string& name, const Value& x1, const Value& x2, const Value& x3,
                  const Value& x4, const Value& x5)
{
	auto it = map5.find(name);
	if(it != map5.end())
	{
		return it->second(x1, x2, x3, x4, x5);
	}
	else
	{
		std::cout << "Couldn't find SysFunc5 " << name << std::endl;
		return Value(0);
	}
}

Value CallSysFunc(const std::string& name, const Value& x1, const Value& x2, const Value& x3,
                  const Value& x4, const Value& x5, const Value& x6)
{
	auto it = map6.find(name);
	if(it != map6.end())
	{
		return it->second(x1, x2, x3, x4, x5, x6);
	}
	else
	{
		std::cout << "Couldn't find SysFunc6 " << name << std::endl;
		return Value(0);
	}
}

// ACTORS
shpvoid Blink(string actorName)
{
    return 0;
}
RegFunc1(Blink, void, string, IMMEDIATE, REL_FUNC);

shpvoid BlinkX(string actorName, string blinkAnim)
{
    return 0;
}
RegFunc2(BlinkX, void, string, string, IMMEDIATE, REL_FUNC);

shpvoid ClearMood(string actorName)
{
    return 0;
}
RegFunc1(ClearMood, void, string, IMMEDIATE, REL_FUNC);

shpvoid EnableEyeJitter(string actorName)
{
    return 0;
}
RegFunc1(EnableEyeJitter, void, string, IMMEDIATE, REL_FUNC);

shpvoid DisableEyeJitter(string actorName)
{
    return 0;
}
RegFunc1(DisableEyeJitter, void, string, IMMEDIATE, REL_FUNC);

shpvoid EyeJitter(string actorName)
{
    return 0;
}
RegFunc1(EyeJitter, void, string, IMMEDIATE, REL_FUNC);

shpvoid DumpActorPosition(string actorName)
{
    return 0;
}
RegFunc1(DumpActorPosition, void, string, IMMEDIATE, DEV_FUNC);

shpvoid Expression(string actorName, string expression)
{
    return 0;
}
RegFunc2(Expression, void, string, string, IMMEDIATE, REL_FUNC);

int GetEgoCurrentLocationCount()
{
    return 1;
}
RegFunc0(GetEgoCurrentLocationCount, int, IMMEDIATE, REL_FUNC);

int GetEgoLocationCount(string locationName)
{
    return 0;
}
RegFunc1(GetEgoLocationCount, int, string, IMMEDIATE, REL_FUNC);

string GetEgoName()
{
	return GEngine::inst->GetScene()->GetEgo()->GetNoun();
}
RegFunc0(GetEgoName, string, IMMEDIATE, REL_FUNC);

std::string GetIndexedPosition(int index)
{
    return "";
}
RegFunc1(GetIndexedPosition, string, int, IMMEDIATE, DEV_FUNC);

int GetPositionCount()
{
    return 0;
}
RegFunc0(GetPositionCount, int, IMMEDIATE, DEV_FUNC);

shpvoid Glance(string actorName, int percentX, int percentY, int durationMs)
{
    return 0;
}

shpvoid GlanceX(string actorName, int leftPercentX, int leftPercentY,
                int rightPercentX, int rightPercentY, int durationMs)
{
    return 0;
}

shpvoid InitEgoPosition(string positionName)
{
    GEngine::inst->GetScene()->InitEgoPosition(positionName);
    return 0;
}
RegFunc1(InitEgoPosition, void, string, IMMEDIATE, REL_FUNC);

int IsActorAtLocation(std::string actorName, std::string locationName)
{
	std::string location = Services::Get<CharacterManager>()->GetCharacterLocation(actorName);
	return StringUtil::EqualsIgnoreCase(location, locationName) ? 1 : 0;
}
RegFunc2(IsActorAtLocation, int, string, string, IMMEDIATE, REL_FUNC);

int IsActorOffstage(string actorName)
{
	return Services::Get<CharacterManager>()->IsCharacterOffstage(actorName) ? 1 : 0;
}
RegFunc1(IsActorOffstage, int, string, IMMEDIATE, REL_FUNC);

int IsCurrentEgo(string actorName)
{
	GKActor* ego = GEngine::inst->GetScene()->GetEgo();
	if(ego == nullptr) { return 0; }
	return StringUtil::EqualsIgnoreCase(ego->GetNoun(), actorName) ? 1 : 0;
}
RegFunc1(IsCurrentEgo, int, string, IMMEDIATE, REL_FUNC);

shpvoid SetActorLocation(string actorName, string locationName)
{
	Services::Get<CharacterManager>()->SetCharacterLocation(actorName, locationName);
	return 0;
}
RegFunc2(SetActorLocation, void, string, string, IMMEDIATE, REL_FUNC);

shpvoid SetActorOffstage(string actorName)
{
	Services::Get<CharacterManager>()->SetCharacterOffstage(actorName);
	return 0;
}
RegFunc1(SetActorOffstage, void, string, IMMEDIATE, REL_FUNC);

shpvoid StartIdleFidget(string actorName) // WAIT
{
	std::cout << "Start idle fidget for " << actorName << std::endl;
	GKActor* actor = GEngine::inst->GetScene()->GetActorByNoun(actorName);
	if(actor != nullptr)
	{
		actor->SetState(GKActor::State::Idle);
	}
	return 0;
}
RegFunc1(StartIdleFidget, void, string, WAITABLE, REL_FUNC);

shpvoid StartListenFidget(string actorName) // WAIT
{
	GKActor* actor = GEngine::inst->GetScene()->GetActorByNoun(actorName);
	if(actor != nullptr)
	{
		actor->SetState(GKActor::State::Listen);
	}
	return 0;
}
RegFunc1(StartListenFidget, void, string, WAITABLE, REL_FUNC);

shpvoid StartTalkFidget(string actorName) // WAIT
{
	GKActor* actor = GEngine::inst->GetScene()->GetActorByNoun(actorName);
	if(actor != nullptr)
	{
		actor->SetState(GKActor::State::Talk);
	}
	return 0;
}
RegFunc1(StartTalkFidget, void, string, WAITABLE, REL_FUNC);

int WasEgoEverInLocation(string locationName)
{
	return 0;
}
RegFunc1(WasEgoEverInLocation, int, string, IMMEDIATE, REL_FUNC);

// ANIMATION AND DIALOGUE
shpvoid StartAnimation(string animationName) // WAIT
{
	std::cout << "Start animation " << animationName << std::endl;
	Animation* animation = Services::GetAssets()->LoadAnimation(animationName);
	if(animation != nullptr)
	{
		SheepVM* currentVM = SheepVM::GetCurrent();
		if(currentVM != nullptr)
		{
			GEngine::inst->GetScene()->GetAnimationPlayer()->Play(animation, currentVM->GetWaitCallback());
		}
		else
		{
			GEngine::inst->GetScene()->GetAnimationPlayer()->Play(animation);
		}
	}
	return 0;
}
RegFunc1(StartAnimation, void, string, WAITABLE, REL_FUNC);

shpvoid StartVoiceOver(string dialogueName, int numLines) // WAIT
{
    string yakName = "E" + dialogueName + ".YAK";
    Yak* yak = Services::GetAssets()->LoadYak(yakName);
    yak->Play(numLines);
    return 0;
}
RegFunc2(StartVoiceOver, void, string, int, WAITABLE, REL_FUNC);

// ENGINE
shpvoid Call(string functionName) // WAIT
{
	Services::GetSheep()->Execute(functionName);
	return 0;
}
RegFunc1(Call, void, string, WAITABLE, REL_FUNC);

shpvoid CallDefaultSheep(string fileName) // WAIT
{
	SheepScript* script = Services::GetAssets()->LoadSheep(fileName);
	if(script != nullptr)
	{
		Services::GetSheep()->Execute(script);
	}
	return 0;
}
RegFunc1(CallDefaultSheep, void, string, WAITABLE, REL_FUNC);

shpvoid CallSheep(string fileName, string functionName) // WAIT
{
	Services::GetSheep()->Execute(fileName, functionName);
    return 0;
}
RegFunc2(CallSheep, void, string, string, WAITABLE, REL_FUNC);

// GAME LOGIC
int GetNounVerbCount(string noun, string verb)
{
	return 0;
}
RegFunc2(GetNounVerbCount, int, string, string, IMMEDIATE, REL_FUNC);

int IsCurrentLocation(string location)
{
	return 0;
}
RegFunc1(IsCurrentLocation, int, string, IMMEDIATE, REL_FUNC);

int IsCurrentTime(string timeCode)
{
    string currentTimeCode = GEngine::inst->GetCurrentTimeCode();
    return StringUtil::EqualsIgnoreCase(currentTimeCode, timeCode);
}
RegFunc1(IsCurrentTime, int, string, IMMEDIATE, REL_FUNC);

int WasLastLocation(string location)
{
	return 0;
}
RegFunc1(WasLastLocation, int, string, IMMEDIATE, REL_FUNC);

int WasLastTime(string timeCode)
{
	return 0;
}
RegFunc1(WasLastTime, int, string, IMMEDIATE, REL_FUNC);

// INVENTORY
int DoesEgoHaveInvItem(string itemName)
{
	return 0;
}
RegFunc1(DoesEgoHaveInvItem, int, string, IMMEDIATE, REL_FUNC);

int DoesGabeHaveInvItem(string itemName)
{
	return 0;
}
RegFunc1(DoesGabeHaveInvItem, int, string, IMMEDIATE, REL_FUNC);

int DoesGraceHaveInvItem(string itemName)
{
    return 0;
}
RegFunc1(DoesGraceHaveInvItem, int, string, IMMEDIATE, REL_FUNC);

// SCENE
shpvoid SetLocation(string location)
{
	GEngine::inst->LoadScene(location);
	return 0;
}
RegFunc1(SetLocation, void, string, WAITABLE, REL_FUNC);
