#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <setjmp.h>

static jmp_buf env_buffer;

void catch_segfault(int signal, siginfo_t *si, void *arg) {
    printf("Caught segfault at address %p (%zu)\n", si->si_addr, (size_t)si->si_addr);
    longjmp(env_buffer, 1);
}

void setup_signal_handler(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_sigaction = catch_segfault;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &sa, NULL);
}

int main(int argc, char *argv[]) {
    setup_signal_handler();

    int val = setjmp(env_buffer);
    if (val != 0) {
        printf("Successfully resumed execution!\n");
    } else {
        printf("Triggering segfault now...\n");
        int *foo = 0;
        printf("Value: %i", *foo);
    }

    printf("After segfault\n");

    return 0;
}
