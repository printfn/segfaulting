#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <setjmp.h>

static jmp_buf env_buffer;

static void catch_segfault(int signal, siginfo_t *si, void *arg) {
    printf("Caught segfault at address %p (%zu)\n", si->si_addr, (size_t)si->si_addr);
    longjmp(env_buffer, 1);
}

static void setup_signal_handler(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_sigaction = catch_segfault;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &sa, NULL);
}

#define DEREF_2(ptr, default_value) (setjmp(env_buffer) ? (default_value) : *(ptr))
#define DEREF_1(ptr) DEREF_2(ptr, 0)
#define DEREF_GET_MACRO(_1, _2, NAME, ...) NAME
#define DEREF(...) DEREF_GET_MACRO(__VA_ARGS__, DEREF_2, DEREF_1)(__VA_ARGS__)

int main(int argc, char *argv[]) {
    setup_signal_handler();

    int an_integer = 42;
    int *an_integer_ptr = &an_integer;
    printf("an_integer: %i\n", DEREF(an_integer_ptr));

    printf("zero: %i\n", DEREF((int *)(an_integer_ptr - &an_integer)));
    printf("random invalid ptr: %i\n", DEREF((int *)782364982735, -1));

    const char *str = "hello";
    printf("string: %s\n", DEREF(&str));
    printf("string2: %s\n", DEREF((char **)9823742763, "segfaulted"));

    return 0;
}
