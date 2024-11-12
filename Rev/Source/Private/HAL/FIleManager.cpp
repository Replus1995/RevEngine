#include "Rev/HAL/FIleManager.h"
#include "PhysFileManager.h"

namespace Rev
{

static IFileManager* SFileManager = nullptr;

IFileManager& IFileManager::Get()
{
    REV_CORE_ASSERT(SFileManager != nullptr);
    return *SFileManager;
}

void IFileManager::Init()
{
    FPhysFileManager::Init();
    SFileManager = new FPhysFileManager;
}

void IFileManager::Cleanup()
{
    SAFE_DELETE(SFileManager);
    FPhysFileManager::Cleanup();
}


}

