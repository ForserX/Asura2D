#include "pch.h"
#include <mio/mio.hpp>

using namespace ark;

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
stl::hash_map<resources::id_t, resource_state> resources_map;
stl::hash_map<resources::id_t, resource_scheduled_task> resource_scheduled_tasks;
std::chrono::nanoseconds last_update_time = {};
std::mutex resource_manager_lock;

auto load_content_context = []()
{
    const auto& content_dir = filesystem::get_content_dir();
    for (const auto& it : std::filesystem::recursive_directory_iterator(content_dir)) {
        if (it.is_regular_file()) {
            std::error_code error;
            auto base_path = std::filesystem::relative(it.path(), content_dir, error).generic_string();
            if (error) {
                ark_assert(false, "Can't setup relative path for file", continue;)
                continue;
            }
            
            resources::load(base_path.data());
        }
    }
    
    last_update_time = std::chrono::steady_clock::now().time_since_epoch();
};

auto resources_scheduled_worker = []()
{
    while (!resources_inited) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    for (const auto& [resource_id, scheduled_task] : resource_scheduled_tasks) {
        bool processed = false;
        if (scheduled_task.task_id == lock_task) {
            processed = resources::lock(resource_id, scheduled_task.begin_offset, scheduled_task.end_offset);
        } else if (scheduled_task.task_id == unlock_task) {
            processed = resources::unlock(resource_id, scheduled_task.begin_offset, scheduled_task.end_offset);
        } else if (scheduled_task.task_id == update_dirs_task) {
            load_content_context();
        }
        
        ark_assert(processed, "Can't process resource scheduled work", continue;)
    }

    {
        std::scoped_lock<std::mutex> scope_lock(resource_manager_lock);
        resource_scheduled_tasks.clear();
    }
        
    return !resources_destroyed;
};

void 
resources::init()
{
    resources_destroyed = false;
    scheduler::schedule(scheduler::global_task_type::resource_manager, resources_scheduled_worker);
    
    load_content_context();
    resources_inited = true;
}

void 
resources::destroy()
{
    resources_destroyed = true;
    resources_inited = false;
}

bool
resources::is_loading()
{
    return !resource_scheduled_tasks.empty();
}

void
resources::update_directories()
{
    resource_scheduled_task resource_task = {};
    resource_task.task_id = update_dirs_task;
    
    if (resource_scheduled_tasks.contains(-1)) {
        return;
    }
    
    resource_scheduled_tasks.emplace(-1, resource_task);
}

const std::chrono::nanoseconds&
resources::get_last_update_time()
{
    return last_update_time;
}

resources::id_t
resources::load(stl::string_view file_name)
{
    id_t resource_id = get_id(file_name);
    if (exists(resource_id)) {
        return resource_id;
    }
    
    auto path = filesystem::get_content_dir();
    path.append(file_name);
    
    resource_state state = {};
    
    std::error_code error;
    state.file_path = std::filesystem::relative(path, filesystem::get_content_dir(), error);
    if (error) {
        return -1;
    }
    
    state.handle = mio::make_mmap_source(path.c_str(), 0, mio::map_entire_file, error);
    if (error) {
        return -1;
    }
    
    resources_map.emplace(resource_id, std::move(state));
    return resource_id;
}
	
void 
resources::unload(id_t resource_id)
{
    if (!exists(resource_id)) {
        return;
    }
    
    resources_map.erase(resource_id);
}
	
bool
resources::schedule_lock(id_t resource_id, int64_t begin_offset, int64_t end_offset)
{
    std::scoped_lock<std::mutex> scope_lock(resource_manager_lock);
    resource_scheduled_task resource_task = {};
    resource_task.begin_offset = begin_offset;
    resource_task.end_offset = end_offset;
    resource_task.task_id = lock_task;
    
    if (resource_scheduled_tasks.contains(resource_id)) {
        return false;
    }
    
    resource_scheduled_tasks.emplace(resource_id, resource_task);
    return true;
}
	
bool
resources::schedule_unlock(id_t resource_id, int64_t begin_offset, int64_t end_offset)
{
    std::scoped_lock scope_lock(resource_manager_lock);
    resource_scheduled_task resource_task = {};
    resource_task.begin_offset = begin_offset;
    resource_task.end_offset = end_offset;
    resource_task.task_id = unlock_task;
    
    if (resource_scheduled_tasks.contains(resource_id)) {
        return false;
    }
    
    resource_scheduled_tasks.emplace(resource_id, resource_task);
    return true;
}

bool
resources::lock(id_t resource_id, int64_t begin_offset, int64_t end_offset)
{
    if (!exists(resource_id)) {
        return false;
    }
    
    const auto& resource = resources_map.at(resource_id);
    const char* mapping_handle = resource.handle.data() + begin_offset;
    
    if (begin_offset == -1) {
        begin_offset = 0;
    }
    
    if (end_offset == -1) {
        end_offset = resource.handle.size();
    }
    
#ifdef _WIN32
    bool locked = !!VirtualLock(const_cast<char*>(mapping_handle), end_offset - begin_offset);
#else
    bool locked = (mlock(mapping_handle, end_offset - begin_offset) == 0);
#endif
    
    return locked;
}
	
bool
resources::unlock(id_t resource_id, int64_t begin_offset, int64_t end_offset)
{
    if (!exists(resource_id)) {
        return false;
    }
    
    const auto& resource = resources_map.at(resource_id);
    const char* mapping_handle = resource.handle.data() + begin_offset;
    
    if (begin_offset == -1) {
        begin_offset = 0;
    }
    
    if (end_offset == -1) {
        end_offset = resource.handle.size();
    }
    
#ifdef _WIN32
    bool unlocked = !!VirtualUnlock(const_cast<char*>(mapping_handle), end_offset - begin_offset);
#else
    bool unlocked = (munlock(mapping_handle, end_offset - begin_offset));
#endif
    
    return unlocked;
}

const char*
resources::get_ptr(id_t resource_id)
{
    if (!exists(resource_id)) {
        return nullptr;
    }
    
    const auto& resource = resources_map.at(resource_id);
    return resource.handle.data();
}

resources::id_t
resources::get_id(std::string_view file_path)
{
    std::hash<std::string_view> hasher;
    return hasher(file_path);
}

uint64_t
resources::get_size(id_t resource_id)
{
    if (!exists(resource_id)) {
        return 0;
    }
    
    const auto& resource = resources_map.at(resource_id);
    return resource.handle.size();
}

stl::string
resources::get_name(id_t resource_id)
{
    if (!exists(resource_id)) {
        return "";
    }
    
    const auto& resource = resources_map.at(resource_id);
    return (stl::string)resource.file_path.generic_string();
}

bool
resources::exists(id_t resource_id)
{
    return resources_map.contains(resource_id);
}

bool
resources::loaded(id_t resource_id)
{
    if (!exists(resource_id)) {
        return false;
    }
    
    const auto& resource = resources_map.at(resource_id);
    return resource.load_state;
}
