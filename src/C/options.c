#include <stdlib.h>

#include "options.h"

#define DELAY_TOTP "1"
#define DELAY_HOTP "3"

int fillflags(modopt* flag, int argc, const char** argv) {
    // Initialisation
    //char line[BUFFER_SIZE];
    char * endptr;
    const char *tmp;
    set_opt(flag, DELAY_TOTP_AUTH, DELAY_TOTP);
    set_opt(flag, DELAY_HOTP_AUTH, DELAY_HOTP);
    
    for (int i =0; i < argc; i++) {
        if (!strcmp("use_auth_tok", argv[i])) {
            set_opt(flag, USE_AUTH_TOK, NULL);
            continue;
        }
        if (!strncmp("delay_totp", argv[i], 10)) {
            if (*(argv[i] + 10) == '=') {
                tmp = argv[i] + 11;
                if (*tmp != '\0') {
                    if (strtol (tmp, &endptr, 10) > 0) {
                        set_opt(flag, DELAY_TOTP_AUTH, tmp);
                    }
                }
            }
            continue;
        }
        if (!strncmp("delay_hotp", argv[i], 10)) {
            if (*(argv[i] + 10) == '=') {
                tmp = argv[i] + 11;
                if (*tmp != '\0') {
                    if (strtol (tmp, &endptr, 10) > 0) {
                        set_opt(flag, DELAY_HOTP_AUTH, tmp);
                    }
                }
            }
            continue;
        }
    }
    return 0;
}

int set_opt(modopt* flag, int field, const char* value) {
    char * endptr;
    switch(field) {
        case USE_AUTH_TOK:
            flag->use_auth_tok = 1;
            break;
        case DELAY_TOTP_AUTH:
            ;
            flag->delay_totp = (uint64_t)(strtol (value, &endptr, 10));
        case DELAY_HOTP_AUTH:
            flag->delay_hotp = (uint64_t)(strtol (value, &endptr, 10));
        default:
            return -1;
    }
    return -1;
}

int is_set(modopt* flag, int field) {
    switch(field) {
        case USE_AUTH_TOK:
            return flag->use_auth_tok;
        default:
            return 0;
    }
    return 0;
}