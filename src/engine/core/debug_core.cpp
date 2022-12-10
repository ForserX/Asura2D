#include "pch.h"

static std::ofstream log_file;

using namespace asura;

#ifdef OS_APPLE_SERIES
#define DebugBreak __builtin_trap

#include <assert.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/sysctl.h>

bool IsDebuggerPresent()
{
    int mib[4];
    struct kinfo_proc info;
    size_t size;

    info.kp_proc.p_flag = 0;
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PID;
    mib[3] = getpid();

    size = sizeof(info);
    sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, NULL, 0);

    return ((info.kp_proc.p_flag & P_TRACED) != 0);
}

void OutputDebugString(const char* data)
{
    if (!IsDebuggerPresent())
        return;

	std::cout << "Asura Engine: " << data << std::endl;
}
#elif defined(OS_LINUX)
bool IsDebuggerPresent()
{
    int pid = fork();
    int res = 0;

    if (pid == -1)
    {
        perror("fork");
        return -1;
    }

    if (pid == 0)
    {
        int ppid = getppid();

        /* Child */
        if (ptrace(PTRACE_ATTACH, ppid, NULL, NULL) == 0)
        {
            /* Wait for the parent to stop and continue it */
            waitpid(ppid, NULL, 0);
            ptrace(PTRACE_CONT, NULL, NULL);

            /* Detach */
            ptrace(PTRACE_DETACH, getppid(), NULL, NULL);

            /* We were the tracers, so gdb is not present */
            res = 0;
        }
        else
        {
            /* Trace failed so GDB is present */
            res = 1;
        }
        exit(res);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
        res = WEXITSTATUS(status);
    }
    return !!res;
}

void DebugBreak()
{
	BYTE bCrash = *(BYTE*)(nullptr);
}

void OutputDebugString(const char* data)
{
    std::cout << "Asura Engine: " << data << std::endl;
}
#endif

void debug::init()
{
	std::filesystem::path log_path = filesystem::get_userdata_dir();
	log_path.append("user.log");

	filesystem::create_file(log_path);

	log_file.open(log_path);
}

void debug::destroy()
{
	log_file.close();
}

void debug::show_error(stl::string_view message)
{
#ifdef _DEBUG
	DebugBreak();
#endif

	print_message(message);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", message.data(), nullptr);
}

void debug::print_message(stl::string_view message)
{
	log_file << message << std::endl;
	ui::push_console_string(message);

#if defined(_DEBUG) & defined(OS_WINDOWS)
	if (IsDebuggerPresent()) {
		OutputDebugString("Asura Engine: ");
		OutputDebugString(message.data());
		OutputDebugString("\r\n");
	}
#else
    OutputDebugString(message.data());
#endif
}