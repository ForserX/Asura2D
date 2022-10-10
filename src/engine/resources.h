#pragma once

namespace ark::resources
{
    using id_type = int64_t;

	void init();
    void destroy();
    bool is_loading();

    id_type load(stl::string_view file_path);
	void unload(id_type resource_id);

	bool schedule_lock(id_type resource_id, int64_t begin_offset = -1, int64_t end_offset = -1);
	bool schedule_unlock(id_type resource_id, int64_t begin_offset = -1, int64_t end_offset = -1);
    bool lock(id_type resource_id, int64_t begin_offset = -1, int64_t end_offset = -1);
	bool unlock(id_type resource_id, int64_t begin_offset = -1, int64_t end_offset = -1);

    const char* get_ptr(id_type resource_id);
    id_type get_id(std::string_view file_name);
    uint64_t get_size(id_type resource_id);
    stl::string get_name(id_type resource_id);

    bool exists(id_type resource_id);
	bool loaded(id_type resource_id);
}
