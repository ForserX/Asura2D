#include "pch.h"

static std::ofstream log_file;

using namespace Asura;

#ifdef OS_APPLE_SERIES
#define DebugBreak __builtin_trap

bool XCodeDebuggerPresent()
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

#define IsDebuggerPresent XCodeDebuggerPresent

#elif defined(OS_LINUX)

bool GDBDebuggerPresent()
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

#define IsDebuggerPresent GDBDebuggerPresent

inline void DebugBreak()
{
    using BYTE = unsigned char;
	BYTE bCrash = *(BYTE*)(nullptr);
}

void OutputDebugString(const char* data)
{
    std::cout << "Asura Engine: " << data << std::endl;
}
#endif

void Debug::Init()
{
	FileSystem::Path log_path = FileSystem::UserdataDir();
	log_path.append("user.log");

	FileSystem::CreateFile(log_path);

	log_file.open(log_path);
}

void Debug::Destroy()
{
	log_file.close();
}

void Debug::show_error(stl::string_view message)
{
	print_message(message);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", message.data(), nullptr);
    
	if (dbg_atttached())
    {
        dbg_break();
    }
}

void Debug::print_message(stl::string_view message)
{
	log_file << message << std::endl;
	UI::push_console_string(message);
    dbg_print(message);
}

void Asura::Debug::dbg_break()
{
    DebugBreak();
}

void Asura::Debug::dbg_print(stl::string_view msg)
{
#if defined(_DEBUG) & defined(OS_WINDOWS)
    if (dbg_atttached()) {
        OutputDebugString("Asura Engine: ");
        OutputDebugString(msg.data());
        OutputDebugString("\r\n");
    }
#else
    if (dbg_atttached())
    {
        OutputDebugString(msg.data());
    }
#endif
}

bool Asura::Debug::dbg_atttached()
{
    return IsDebuggerPresent();
}
