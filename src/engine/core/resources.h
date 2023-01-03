#pragma once

namespace Asura
{
    struct Resource
    {
        size_t Size;
        ptrdiff_t ID;
        const char* Ptr;
        stl::string Name;
    };

    class ResourceScopeLock
    {
        Resource* Res;

    public:
        ResourceScopeLock() = delete;
        ResourceScopeLock(Resource& Ref);
        ~ResourceScopeLock();
    };
}

namespace Asura::ResourcesManager
{
    using id_t = int64_t;

	void Init();
    void Destroy();
    bool IsLoading();

    void UpdateDirs();
    const std::chrono::nanoseconds& LastUpdateTime();

    id_t Load(FileSystem::Path file_path);
	void Unload(id_t resource_id);

	bool ScheduleLock(id_t resource_id, int64_t begin_offset = -1, int64_t end_offset = -1);
	bool ScheduleUnlock(id_t resource_id, int64_t begin_offset = -1, int64_t end_offset = -1);
    bool Lock(id_t resource_id, int64_t begin_offset = -1, int64_t end_offset = -1);
	bool Unlock(id_t resource_id, int64_t begin_offset = -1, int64_t end_offset = -1);

    const char* GetPtr(id_t resource_id);
    id_t GetID(std::string_view file_name);
    uint64_t GetSize(id_t resource_id);
    stl::string GetName(id_t resource_id);

    Resource GetResource(id_t resource_id);

    bool Exists(id_t resource_id);
	bool Loaded(id_t resource_id);
}
