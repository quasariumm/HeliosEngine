#pragma once
#include <filesystem>
#include <cereal/cereal.hpp>

namespace Engine::Resources
{

struct Resource
{
    Resource()          = default;
    virtual ~Resource() = default;
    
    /**
     * Load the resource via overridden function
     * @return True if loading succeeded else false
     */
    virtual bool Load() = 0;

    /**
     * Save the resource via overridden function
     * @return True if saving succeeded else false
     * @note Some resources may not be savable and will always return false
     */
    virtual bool Save() { Log::Error("This type of resource is not allowed to be saved"); return false; }
    
    std::filesystem::path filePath;
    uint32_t id;
    uint32_t refCount;
};

inline std::vector<std::shared_ptr<Resource>> loadedResources = {};
inline uint32_t highestID = 0;

void DecreaseRefCount(uint32_t id);
bool IsResourceLoaded(const std::filesystem::path& filePath);

template <typename T>
struct ResourceHandle
{
    ResourceHandle() = default;
    ResourceHandle(const std::shared_ptr<Resource>& resource) { rawData = resource; }
    ~ResourceHandle() { DecreaseRefCount(rawData->id); }
    
    std::shared_ptr<Resource> rawData = nullptr;
    T* Data() { return reinterpret_cast<T*>(rawData.get()); }
    std::filesystem::path Path() const { return rawData->filePath; }
    uint32_t ID() const { return rawData->id; }

    // TODO(Quillan): Figure out a way to automatically serialize resources. Maybe using visit struct?

    // template <typename Archive>
    // void Serialize(Archive ar)
    // {
    //     ar(CEREAL_NVP(rawData->filePath));
    // }
};

template<typename T>
ResourceHandle<T> LoadResource(const std::filesystem::path& filePath)
{
    if (!std::filesystem::exists(filePath))
    {
        Log::Error("Requested resource not found: " + filePath.string() );
        return ResourceHandle<T>();
    }

    if (IsResourceLoaded(filePath))
    {
        for (auto & loadedResource : loadedResources)
        {
            if (loadedResource)
                return ResourceHandle<T>(loadedResource);
        }
    }
    
    auto resource = std::make_shared<T>();
    resource->id = highestID++;
    resource->refCount = 1;
    resource->filePath = std::move(filePath);
    
    if (!resource->Load())
    {
        Log::Error("Failed to load resource '{}'", resource->filePath.generic_string());
        return ResourceHandle<T>();
    }
    
    loadedResources.push_back(resource);

    return ResourceHandle<T>(resource);
}

}
