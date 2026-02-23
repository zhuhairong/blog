#include "signal_utils.h"
#include <string.h>
#include <stdlib.h>

static volatile sig_atomic_t g_exit_requested = 0;
static signal_state_t g_state = {0};

void signal_config_init(signal_config_t *config) {
    if (!config) return;
    memset(config, 0, sizeof(signal_config_t));
    config->catch_sigint = true;
    config->catch_sigterm = true;
    config->catch_sigquit = false;
    config->catch_sighup = false;
    config->catch_sigpipe = false;
    config->catch_sigsegv = false;
    config->use_async_safe = true;
    config->enable_nested_handlers = false;
    config->max_handlers = 32;
}

void signal_state_init(signal_state_t *state) {
    if (!state) return;
    memset(state, 0, sizeof(signal_state_t));
    state->is_initialized = true;
}

static void default_signal_handler(int sig, siginfo_t *info, void *context) {
    (void)info;
    (void)context;
    
    g_exit_requested = 1;
    g_state.exit_requested = true;
    g_state.pending_signal = sig;
    g_state.signal_count++;
}

void signal_init_exit_handler(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = (void (*)(int))default_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    
    g_state.is_initialized = true;
}

signal_error_t signal_init_ex(const signal_config_t *config, signal_state_t *state) {
    signal_config_t cfg;
    if (config) {
        cfg = *config;
    } else {
        signal_config_init(&cfg);
    }
    
    signal_state_init(&g_state);
    
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = default_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    
    if (cfg.catch_sigint) {
        if (sigaction(SIGINT, &sa, NULL) < 0) {
            return SIGNAL_ERROR_REGISTRATION;
        }
    }
    
    if (cfg.catch_sigterm) {
        if (sigaction(SIGTERM, &sa, NULL) < 0) {
            return SIGNAL_ERROR_REGISTRATION;
        }
    }
    
    if (cfg.catch_sigquit) {
        if (sigaction(SIGQUIT, &sa, NULL) < 0) {
            return SIGNAL_ERROR_REGISTRATION;
        }
    }
    
    if (cfg.catch_sighup) {
        if (sigaction(SIGHUP, &sa, NULL) < 0) {
            return SIGNAL_ERROR_REGISTRATION;
        }
    }
    
    if (cfg.catch_sigpipe) {
        if (sigaction(SIGPIPE, &sa, NULL) < 0) {
            return SIGNAL_ERROR_REGISTRATION;
        }
    }
    
    if (state) {
        *state = g_state;
    }
    
    return SIGNAL_OK;
}

bool signal_is_exit_requested(void) {
    return g_exit_requested != 0;
}

signal_error_t signal_get_exit_status(bool *exit_requested, signal_state_t *state) {
    if (!exit_requested) return SIGNAL_ERROR_INVALID_PARAMS;
    
    *exit_requested = g_exit_requested != 0;
    
    if (state) {
        *state = g_state;
    }
    
    return SIGNAL_OK;
}

signal_error_t signal_register_handler(int signum, signal_handler_t handler, signal_state_t *state) {
    if (!handler) return SIGNAL_ERROR_HANDLER_NULL;
    if (signum <= 0 || signum >= NSIG) return SIGNAL_ERROR_SIGNAL_INVALID;
    
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = (void (*)(int, siginfo_t *, void *))handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    
    if (sigaction(signum, &sa, NULL) < 0) {
        return SIGNAL_ERROR_REGISTRATION;
    }
    
    g_state.signal_count++;
    
    if (state) {
        *state = g_state;
    }
    
    return SIGNAL_OK;
}

signal_error_t signal_remove_handler(int signum, signal_state_t *state) {
    if (signum <= 0 || signum >= NSIG) return SIGNAL_ERROR_SIGNAL_INVALID;
    
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    
    if (sigaction(signum, &sa, NULL) < 0) {
        return SIGNAL_ERROR_REGISTRATION;
    }
    
    if (state) {
        *state = g_state;
    }
    
    return SIGNAL_OK;
}

signal_error_t signal_block(int signum, signal_state_t *state) {
    if (signum <= 0 || signum >= NSIG) return SIGNAL_ERROR_SIGNAL_INVALID;
    
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, signum);
    
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
        return SIGNAL_ERROR_DISABLE;
    }
    
    if (state) {
        *state = g_state;
    }
    
    return SIGNAL_OK;
}

signal_error_t signal_unblock(int signum, signal_state_t *state) {
    if (signum <= 0 || signum >= NSIG) return SIGNAL_ERROR_SIGNAL_INVALID;
    
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, signum);
    
    if (sigprocmask(SIG_UNBLOCK, &mask, NULL) < 0) {
        return SIGNAL_ERROR_DISABLE;
    }
    
    if (state) {
        *state = g_state;
    }
    
    return SIGNAL_OK;
}

signal_error_t signal_ignore(int signum, signal_state_t *state) {
    if (signum <= 0 || signum >= NSIG) return SIGNAL_ERROR_SIGNAL_INVALID;
    
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    
    if (sigaction(signum, &sa, NULL) < 0) {
        return SIGNAL_ERROR_REGISTRATION;
    }
    
    if (state) {
        *state = g_state;
    }
    
    return SIGNAL_OK;
}

signal_error_t signal_restore_default(int signum, signal_state_t *state) {
    if (signum <= 0 || signum >= NSIG) return SIGNAL_ERROR_SIGNAL_INVALID;
    
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    
    if (sigaction(signum, &sa, NULL) < 0) {
        return SIGNAL_ERROR_REGISTRATION;
    }
    
    if (state) {
        *state = g_state;
    }
    
    return SIGNAL_OK;
}

signal_error_t signal_send(pid_t pid, int signum, signal_state_t *state) {
    if (signum <= 0 || signum >= NSIG) return SIGNAL_ERROR_SIGNAL_INVALID;
    
    if (kill(pid, signum) < 0) {
        return SIGNAL_ERROR_REGISTRATION;
    }
    
    if (state) {
        *state = g_state;
    }
    
    return SIGNAL_OK;
}

const char* signal_get_name(int signum) {
    switch (signum) {
        case SIGINT: return "SIGINT";
        case SIGTERM: return "SIGTERM";
        case SIGQUIT: return "SIGQUIT";
        case SIGHUP: return "SIGHUP";
        case SIGPIPE: return "SIGPIPE";
        case SIGSEGV: return "SIGSEGV";
        case SIGABRT: return "SIGABRT";
        case SIGFPE: return "SIGFPE";
        case SIGILL: return "SIGILL";
        case SIGKILL: return "SIGKILL";
        case SIGSTOP: return "SIGSTOP";
        case SIGCONT: return "SIGCONT";
        case SIGUSR1: return "SIGUSR1";
        case SIGUSR2: return "SIGUSR2";
        case SIGALRM: return "SIGALRM";
        case SIGCHLD: return "SIGCHLD";
        default: return "UNKNOWN";
    }
}

const char* signal_get_description(int signum) {
    switch (signum) {
        case SIGINT: return "Interrupt from keyboard";
        case SIGTERM: return "Termination signal";
        case SIGQUIT: return "Quit from keyboard";
        case SIGHUP: return "Hangup detected on controlling terminal";
        case SIGPIPE: return "Broken pipe: write to pipe with no readers";
        case SIGSEGV: return "Invalid memory reference";
        case SIGABRT: return "Abort signal from abort()";
        case SIGFPE: return "Floating point exception";
        case SIGILL: return "Illegal Instruction";
        case SIGKILL: return "Kill signal";
        case SIGSTOP: return "Stop process";
        case SIGCONT: return "Continue if stopped";
        case SIGUSR1: return "User-defined signal 1";
        case SIGUSR2: return "User-defined signal 2";
        case SIGALRM: return "Timer signal from alarm()";
        case SIGCHLD: return "Child stopped or terminated";
        default: return "Unknown signal";
    }
}

signal_error_t signal_reset(signal_state_t *state) {
    g_exit_requested = 0;
    signal_state_init(&g_state);
    
    if (state) {
        *state = g_state;
    }
    
    return SIGNAL_OK;
}

signal_error_t signal_disable_all(signal_state_t *state) {
    sigset_t mask;
    sigfillset(&mask);
    
    if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
        return SIGNAL_ERROR_DISABLE;
    }
    
    if (state) {
        *state = g_state;
    }
    
    return SIGNAL_OK;
}

const char* signal_strerror(const signal_state_t *state) {
    if (!state) return "Invalid state";
    
    switch (state->last_error) {
        case SIGNAL_OK: return "Success";
        case SIGNAL_ERROR_INVALID_PARAMS: return "Invalid parameters";
        case SIGNAL_ERROR_REGISTRATION: return "Signal registration failed";
        case SIGNAL_ERROR_DISABLE: return "Signal disable failed";
        case SIGNAL_ERROR_HANDLER_NULL: return "Handler is null";
        case SIGNAL_ERROR_SIGNAL_INVALID: return "Invalid signal number";
        case SIGNAL_ERROR_MEMORY: return "Memory allocation error";
        default: return "Unknown error";
    }
}
