//
// Clark Kromenaker
//
// Manages loading and caching of assets.
//
#pragma once
#include <functional>
#include <initializer_list>
#include <string>
#include <vector>

#include "Asset.h"
#include "StringUtil.h"

class BarnFile;

// Forward Declarations for all asset types
class Animation;
class Audio;
class BSP;
class BSPLightmap;
class Config;
class Cursor;
class Font;
class GAS;
class Model;
class NVC;
class SceneAsset;
class SceneInitFile;
class Sequence;
class Shader;
class SheepScript;
class Soundtrack;
class TextAsset;
class Texture;
class VertexAnimation;

class AssetManager
{
public:
    AssetManager();
    ~AssetManager();

    // Loose Files
	// Adds a filesystem path to search for assets and bundles at.
    void AddSearchPath(const std::string& searchPath);
    
    // Given a filename, finds the path to the file if it exists on one of the search paths.
    // Returns empty string if file is not found.
    std::string GetAssetPath(const std::string& fileName);
    std::string GetAssetPath(const std::string& fileName, std::initializer_list<std::string> extensions);

    // Barn Files
	// Load or unload a barn bundle.
    bool LoadBarn(const std::string& barnName);
    void UnloadBarn(const std::string& barnName);
	
	// Write an asset from a bundle to a file.
    void WriteBarnAssetToFile(const std::string& assetName);
	void WriteBarnAssetToFile(const std::string& assetName, const std::string& outputDir);
	
	// Write all assets from a bundle that match a search string.
	void WriteAllBarnAssetsToFile(const std::string& search);
	void WriteAllBarnAssetsToFile(const std::string& search, const std::string& outputDir);

    // Loading (or Getting) Assets
    Audio* LoadAudio(const std::string& name, AssetScope scope = AssetScope::Global);
    Soundtrack* LoadSoundtrack(const std::string& name, AssetScope scope = AssetScope::Global);
	Animation* LoadYak(const std::string& name, AssetScope scope = AssetScope::Global);
    
    Model* LoadModel(const std::string& name, AssetScope scope = AssetScope::Global);
    Texture* LoadTexture(const std::string& name, AssetScope scope = AssetScope::Global);
    Texture* LoadSceneTexture(const std::string& name, AssetScope scope = AssetScope::Global);
    const std::unordered_map_ci<std::string, Texture*>& GetLoadedTextures() { return mLoadedTextures; }
    
    GAS* LoadGAS(const std::string& name, AssetScope scope = AssetScope::Global);
    Animation* LoadAnimation(const std::string& name, AssetScope scope = AssetScope::Global);
    Animation* LoadMomAnimation(const std::string& name, AssetScope scope = AssetScope::Global);
    VertexAnimation* LoadVertexAnimation(const std::string& name, AssetScope scope = AssetScope::Global);
    Sequence* LoadSequence(const std::string& name, AssetScope scope = AssetScope::Global);
    
    SceneInitFile* LoadSIF(const std::string& name, AssetScope scope = AssetScope::Global);
    SceneAsset* LoadSceneAsset(const std::string& name, AssetScope scope = AssetScope::Global);
    NVC* LoadNVC(const std::string& name, AssetScope scope = AssetScope::Global);
    
    BSP* LoadBSP(const std::string& name, AssetScope scope = AssetScope::Global);
    BSPLightmap* LoadBSPLightmap(const std::string& name, AssetScope scope = AssetScope::Global);
    
    SheepScript* LoadSheep(const std::string& name, AssetScope scope = AssetScope::Global);
    
    Cursor* LoadCursor(const std::string& name, AssetScope scope = AssetScope::Global);
	Font* LoadFont(const std::string& name, AssetScope scope = AssetScope::Global);
	
    TextAsset* LoadText(const std::string& name, AssetScope scope = AssetScope::Global);
    Config* LoadConfig(const std::string& name);

    Shader* LoadShader(const std::string& name);
    Shader* LoadShader(const std::string& vertName, const std::string& fragName);

    // Unloading Assets
    void UnloadAssets(AssetScope scope);
    //TODO: Unloading individual assets?
    //void UnloadAsset(Asset* asset);
    //template<typename T> void UnloadAsset(T* asset);
    
private:
    // A list of paths to search for assets.
    // In priority order, since we'll search in order, and stop when we find the item.
    std::vector<std::string> mSearchPaths;
    
    // A map of loaded barn files. If an asset isn't found on any search path,
    // we then search each loaded barn file for the asset.
    std::unordered_map_ci<std::string, BarnFile> mLoadedBarns;
    
    // A list of loaded assets, so we can just return existing assets if already loaded.
    std::unordered_map_ci<std::string, Audio*> mLoadedAudios;
	std::unordered_map_ci<std::string, Soundtrack*> mLoadedSoundtracks;
	std::unordered_map_ci<std::string, Animation*> mLoadedYaks;
	
	std::unordered_map_ci<std::string, Model*> mLoadedModels;
    std::unordered_map_ci<std::string, Texture*> mLoadedTextures;
	
	std::unordered_map_ci<std::string, GAS*> mLoadedGases;
	std::unordered_map_ci<std::string, Animation*> mLoadedAnimations;
    std::unordered_map_ci<std::string, Animation*> mLoadedMomAnimations;
	std::unordered_map_ci<std::string, VertexAnimation*> mLoadedVertexAnimations;
    std::unordered_map_ci<std::string, Sequence*> mLoadedSequences;
	
	std::unordered_map_ci<std::string, SceneInitFile*> mLoadedSIFs;
	std::unordered_map_ci<std::string, SceneAsset*> mLoadedSceneAssets;
	std::unordered_map_ci<std::string, NVC*> mLoadedActionSets;
    
	std::unordered_map_ci<std::string, BSP*> mLoadedBSPs;
    std::unordered_map_ci<std::string, BSPLightmap*> mLoadedBSPLightmaps;
    
	std::unordered_map_ci<std::string, SheepScript*> mLoadedSheeps;

    std::unordered_map_ci<std::string, Cursor*> mLoadedCursors;
    std::unordered_map_ci<std::string, Font*> mLoadedFonts;
	
    std::unordered_map_ci<std::string, TextAsset*> mLoadedTexts;
    std::unordered_map_ci<std::string, Config*> mLoadedConfigs;

    std::unordered_map_ci<std::string, Shader*> mLoadedShaders;
	
	// Retrieve a barn bundle by name, or by contained asset.
	BarnFile* GetBarn(const std::string& barnName);
	BarnFile* GetBarnContainingAsset(const std::string& assetName);
    
    std::string SanitizeAssetName(const std::string& assetName, const std::string& expectedExtension);

    // Two ways to load an asset:
    // The first uses a single constructor (name, data, size).
    // The second uses a constructor (name) and a separate load function (data, size).
    // The latter is necessary if two assets can potentially attempt to load one another (circular dependency).
    template<class T> T* LoadAsset(const std::string& assetName, AssetScope scope, std::unordered_map_ci<std::string, T*>* cache, T*(*createFunc)(const std::string&, AssetScope, char*, unsigned int) = nullptr, bool deleteBuffer = true);
    template<class T> T* LoadAsset_SeparateLoadFunc(const std::string& assetName, AssetScope scope, std::unordered_map_ci<std::string, T*>* cache, bool deleteBuffer = true);
    
    char* CreateAssetBuffer(const std::string& assetName, unsigned int& outBufferSize);

    template<class T> void UnloadAsset(T* asset, std::unordered_map_ci<std::string, T*>* cache = nullptr);
    template<class T> void UnloadAssets(std::unordered_map_ci<std::string, T*>& cache, AssetScope scope = AssetScope::Global);
};

extern AssetManager gAssetManager;