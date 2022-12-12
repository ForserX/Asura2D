#include "pch.h"
#include <mio/mio.hpp>

using namespace Asura;

struct resource_state
{
    bool load_state;
    mio::mmap_source handle;
    std::filesystem::path file_path;
};

struct resource_scheduled_task
{
    int32_t task_id;
    int64_t begin_offset;
    int64_t end_offset;
};

enum resource_task_enum
{
    lock_task,
    unlock_task,
    update_dirs_task
};

bool resources_inited = false;
bool resources_destroyed = false;
stl::hash_map<ResourcesManager::id_t, resource_state> resources_map;
stl::hash_map<ResourcesManager::id_t, resource_scheduled_task> resource_scheduled_tasks;
std::chrono::nanoseconds last_update_time = {};
std::mutex resource_manager_lock;

auto load_content_context = []()
{
    const auto& content_dir = FileSystem::ContentDir();
    for (const auto& it : std::filesystem::recursive_directory_iterator(content_dir))
    {
        if (it.is_regular_file())
        {
            std::error_code error;
            auto base_path = std::filesystem::relative(it.path(), content_dir, error).generic_string();

            // #TODO: Rework
            if (error) 
            {
                game_assert(false, "Can't setup relative path for file", continue;)
                continue;
            }
            
            ResourcesManager::Load(base_path.data());
        }
    }
    
    last_update_time = std::chrono::steady_clock::now().time_since_epoch();
};

auto resources_scheduled_worker = []()
{
    while (!resources_inited) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    for (const auto& [resource_id, scheduled_task] : resource_scheduled_tasks) 
    {
        bool processed = false;
        if (scheduled_task.task_id == lock_task) 
        {
            processed = ResourcesManager::Lock(resource_id, scheduled_task.begin_offset, scheduled_task.end_offset);
        } else if (scheduled_task.task_id == unlock_task) {
            processed = ResourcesManager::Unlock(resource_id, scheduled_task.begin_offset, scheduled_task.end_offset);
        } else if (scheduled_task.task_id == update_dirs_task) {
            load_content_context();
        }
        
        game_assert(processed, "Can't process resource scheduled work", continue;)
    }

    {
        std::scoped_lock<std::mutex> scope_lock(resource_manager_lock);
        resource_scheduled_tasks.clear();
    }
        
    return !resources_destroyed;
};

void ResourcesManager::Init()
{
    resources_destroyed = false;
    Scheduler::schedule(Scheduler::global_task_type::resource_manager, resources_scheduled_worker);
    
    load_content_context();
    resources_inited = true;
}

void ResourcesManager::Destroy()
{
    resources_destroyed = true;
    resources_inited = false;
}

bool ResourcesManager::IsLoading()
{
    return !resource_scheduled_tasks.empty();
}

void ResourcesManager::UpdateDirs()
{
    resource_scheduled_task resource_task = {};
    resource_task.task_id = update_dirs_task;
    
    if (resource_scheduled_tasks.contains(-1)) 
    {
        return;
    }
    
    resource_scheduled_tasks.emplace(-1, resource_task);
}

const std::chrono::nanoseconds& ResourcesManager::LastUpdateTime()
{
    return last_update_time;
}

ResourcesManager::id_t ResourcesManager::Load(FileSystem::Path file_name)
{
    id_t resource_id = GetID(file_name.generic_string().c_str());
    if (Exists(resource_id)) 
    {
        return resource_id;
    }
    
    auto path = FileSystem::ContentDir() / file_name;

    resource_state state = {};
    
    std::error_code error;
    state.file_path = std::filesystem::relative(path, FileSystem::ContentDir(), error);

    if (error) 
    {
        return -1;
    }
    
    state.handle = mio::make_mmap_source(path.c_str(), 0, mio::map_entire_file, error);
    if (error) 
    {
        return -1;
    }
    
    resources_map.emplace(resource_id, std::move(state));
    return resource_id;
}
	
void ResourcesManager::Unload(id_t resource_id)
{
    if (!Exists(resource_id))
    {
        return;
    }
    
    resources_map.erase(resource_id);
}
	
bool ResourcesManager::ScheduleLock(id_t resource_id, int64_t begin_offset, int64_t end_offset)
{
    std::scoped_lock<std::mutex> scope_lock(resource_manager_lock);
    resource_scheduled_task resource_task = {};
    resource_task.begin_offset = begin_offset;
    resource_task.end_offset = end_offset;
    resource_task.task_id = lock_task;
    
    if (resource_scheduled_tasks.contains(resource_id))
    {
        return false;
    }
    
    resource_scheduled_tasks.emplace(resource_id, resource_task);
    return true;
}
	
bool ResourcesManager::ScheduleUnlock(id_t resource_id, int64_t begin_offset, int64_t end_offset)
{
    std::scoped_lock scope_lock(resource_manager_lock);
    resource_scheduled_task resource_task = {};
    resource_task.begin_offset = begin_offset;
    resource_task.end_offset = end_offset;
    resource_task.task_id = unlock_task;
    
    if (resource_scheduled_tasks.contains(resource_id))
    {
        return false;
    }
    
    resource_scheduled_tasks.emplace(resource_id, resource_task);
    return true;
}

bool ResourcesManager::Lock(id_t resource_id, int64_t begin_offset, int64_t end_offset)
{
    if (!Exists(resource_id)) 
    {
        return false;
    }
    
    const auto& resource = resources_map.at(resource_id);
    const char* mapping_handle = resource.handle.data() + begin_offset;
    
    if (begin_offset == -1) 
    {
        begin_offset = 0;
    }
    
    if (end_offset == -1) 
    {
        end_offset = resource.handle.size();
    }
    
#ifdef OS_WINDOWS
    bool locked = !!VirtualLock(const_cast<char*>(mapping_handle), end_offset - begin_offset);
#else
    bool locked = (mlock(mapping_handle, end_offset - begin_offset) == 0);
#endif
    
    return locked;
}
	
bool ResourcesManager::Unlock(id_t resource_id, int64_t begin_offset, int64_t end_offset)
{
    if (!Exists(resource_id)) 
    {
        return false;
    }
    
    const auto& resource = resources_map.at(resource_id);
    const char* mapping_handle = resource.handle.data() + begin_offset;
    
    if (begin_offset == -1) 
    {
        begin_offset = 0;
    }
    
    if (end_offset == -1) 
    {
        end_offset = resource.handle.size();
    }
    
#ifdef OS_WINDOWS
    bool unlocked = !!VirtualUnlock(const_cast<char*>(mapping_handle), end_offset - begin_offset);
#else
    bool unlocked = (munlock(mapping_handle, end_offset - begin_offset));
#endif
    
    return unlocked;
}

const char* ResourcesManager::GetPtr(id_t resource_id)
{
    if (!Exists(resource_id))
    {
        return nullptr;
    }
    
    const auto& resource = resources_map.at(resource_id);
    return resource.handle.data();
}

ResourcesManager::id_t ResourcesManager::GetID(std::string_view file_path)
{
    std::hash<std::string_view> hasher;
    return hasher(file_path);
}

uint64_t ResourcesManager::GetSize(id_t resource_id)
{
    if (!Exists(resource_id)) 
    {
        return 0;
    }
    
    const auto& resource = resources_map.at(resource_id);
    return resource.handle.size();
}

stl::string ResourcesManager::GetName(id_t resource_id)
{
    if (!Exists(resource_id)) 
    {
        return "";
    }
    
    const auto& resource = resources_map.at(resource_id);
    return (stl::string)resource.file_path.generic_string();
}

Resource Asura::ResourcesManager::GetResource(id_t resource_id)
{
    Resource Res = {};
    Res.ID = resource_id;
    Res.Name = GetName(resource_id);
    Res.Ptr = GetPtr(resource_id);
    Res.Size = GetSize(resource_id);

    return Res;
}

bool ResourcesManager::Exists(id_t resource_id)
{
    return resources_map.contains(resource_id);
}

bool ResourcesManager::Loaded(id_t resource_id)
{
    if (!Exists(resource_id)) 
    {
        return false;
    }
    
    const auto& resource = resources_map.at(resource_id);
    return resource.load_state;
}

Asura::ResourceScopeLock::ResourceScopeLock(Resource& Ref)
{
    Res = &Ref;
    ResourcesManager::Lock(Res->ID);
}

Asura::ResourceScopeLock::~ResourceScopeLock()
{
    ResourcesManager::Unlock(Res->ID);
}
