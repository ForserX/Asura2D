#pragma once

namespace ark::resources
{
	void init();
    void destroy();

	int32_t load(std::string_view file_path);
	void unload(int32_t resource_id);

	bool schedule_lock(int32_t resource_id, int64_t begin_offset = -1, int64_t end_offset = -1);
	bool schedule_unlock(int32_t resource_id, int64_t begin_offset = -1, int64_t end_offset = -1);
	void lock(int32_t resource_id, int64_t begin_offset = -1, int64_t end_offset = -1);
	void unlock(int32_t resource_id, int64_t begin_offset = -1, int64_t end_offset = -1);

    const char* ptr(int32_t resource_id);
	int32_t id(std::string_view file_name);
    std::string_view name(int32_t resource_id);

    bool exists(int32_t resource_id);
	bool loaded(int32_t resource_id);
}
