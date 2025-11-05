#include "core/resources.hpp"

using namespace Helios;
using namespace Resources;


void Resources::DecreaseRefCount( uint32_t id )
{
	for (int i = 0; i < static_cast<int>(loadedResources.size()); i++)
	{
		auto resource = loadedResources[i];
		if (resource->id == id)
		{
			resource->refCount--;
			if (resource->refCount == 0)
				loadedResources.erase(loadedResources.begin() + i);
			return;
		}
	}
}


bool Resources::IsResourceLoaded( const std::filesystem::path& filePath )
{
	for (const auto& loadedResource : loadedResources)
	{
		if (loadedResource->filePath == filePath)
			return true;
	}
	return false;
}
