#pragma once

#if defined(_WIN32)
#define KUAS_PLATFORM_WINDOWS 1
#elif defined(__unix__)
#define KUAS_PLATFORM_UNIX_LIKE 1
#ifdef __linux__
#define KUAS_PLATFORM_LINUX 1
#elif defined(__ANDROID__)
#define KUAS_PLATFORM_ANDROID 1
#endif
#endif

// always define this for now
#define KUAS_USE_SDL 1
