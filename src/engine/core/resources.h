#pragma once

namespace Asura::resources
{
    using id_t = int64_t;

	void Init();
    void Destroy();
    bool is_loading();

    void update_directories();
    const std::chrono::nanoseconds& get_last_update_time();

    id_t load(stl::string_view file_path);
	void unload(id_t resource_id);

	bool schedule_lock(id_t resource_id, int64_t begin_offset = -1, int64_t end_offset = -1);
	bool schedule_unlock(id_t resource_id, int64_t begin_offset = -1, int64_t end_offset = -1);
    bool lock(id_t resource_id, int64_t begin_offset = -1, int64_t end_offset = -1);
	bool unlock(id_t resource_id, int64_t begin_offset = -1, int64_t end_offset = -1);

    const char* get_ptr(id_t resource_id);
    id_t get_id(std::string_view file_name);
    uint64_t get_size(id_t resource_id);
    stl::string get_name(id_t resource_id);

    bool exists(id_t resource_id);
	bool loaded(id_t resource_id);
}
