#include <cassert>
#include <fcntl.h>
#include <sys/resource.h>
#include <unistd.h>
#include <cstdio>

int main() {
    const char* path = "test_f_nocache.tmp";
    int fd = ::open(path, O_RDWR | O_CREAT | O_TRUNC, 0644);
    assert(fd >= 0);

#ifdef __APPLE__
    int ret = ::fcntl(fd, F_NOCACHE, 1);
    assert(ret == 0);
#endif

    ::close(fd);
    ::unlink(path);

    rlimit limit{};
    int r = ::getrlimit(RLIMIT_NOFILE, &limit);
    assert(r == 0);
    assert(limit.rlim_cur >= 256);

    return 0;
}
