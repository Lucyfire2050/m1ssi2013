#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define _PAM_EXTERN_FUNCTIONS
#include <_pam_macros.h>
#include <pam_modules.h>
#include <pam_ext.h>

#include <sys/syslog.h>
#include <string.h>
#include <strings.h>
#include <pwd.h>

#include "utils/otp.h"
#include "utils/users.h"
#include "options.h"
#include "tools/conv.h"

#define DELAY_TOTP 1
#define DELAY_HOTP 3

#define OTP_MAX_LENGTH 8

/** Vérifie que l'otp est valide pour l'utilisateur passé en paramètre.
 *
 */
int _check_totp(pam_handle_t * pamh, otpuser * user,
                const char * otp, uint16_t delay) {
    // Vérification d'un mot de passe.
    int otp_expected = 0;
    int otp_given = atoi(otp);
    long lastAuth = user->params.totp.tps;
    int hasFound = 0;
    for (int i = -2; i <= 3 && !hasFound; i++) {
        int delta = (user->params.totp.delay + i) * user->params.totp.quantum;
        long counter = (time(NULL) + delta) / user->params.totp.quantum;
        if (user->params.totp.tplstauth < time(NULL)) {
            if (lastAuth < counter) {
                otp_expected = generate_otp(user->passwd, counter, user->otp_len);
                if (otp_expected < OTP_SUCCESS) {
                    pam_syslog(pamh, LOG_ERR, "TOTP generation failed errcode=%d",
                            otp_expected);
                    unlockFile();
                    return PAM_AUTH_ERR;
                }
                if (otp_expected == otp_given) {
                    hasFound = 1;
                    user->params.totp.tps = counter;
                    user->params.totp.delay += i;
                    updateOTPUser(user);
                    pam_syslog(pamh, LOG_NOTICE, "%s logged in", user->username);
                    return PAM_SUCCESS;
                }
            }
        } else {
            pam_info(pamh, "Veuillez reessayer dans %ld secondes.",
                     (user->params.totp.tplstauth - time(NULL)));
            pam_syslog(pamh, LOG_ERR, "Nouvelle tentative trop rapide.");
            return PAM_AUTH_ERR;
        }
    }
    user->params.totp.tplstauth = time(NULL) + delay;
    updateOTPUser(user);
    pam_syslog(pamh, LOG_NOTICE, "%s failed to log in", user->username);
    if (!hasFound) {
        pam_syslog(pamh, LOG_ERR, "can't synchronize");
    }
    return PAM_AUTH_ERR;
}

/** Vérifie que qu'un utilisateur entre le bon OTP.
 */
int _check_hotp(pam_handle_t * pamh, otpuser * user, const char * otp, uint64_t delay) {
    // Vérification d'un mot de passe + resynch.
    int otp_expected = 0;
    int otp_given = atoi(otp);

    if (user->params.hotp.tplstauth < time(NULL)) {
        for (int i = 0; i < 3; i++) {
            otp_expected = generate_otp(user->passwd, user->params.hotp.count + i,
                                        user->otp_len);
            if (otp_expected < OTP_SUCCESS) {
                pam_syslog(pamh, LOG_ERR, "generate_otp failed");
                unlockFile();
                return PAM_AUTH_ERR;
            }
            
            if (otp_expected == otp_given) {
                user->params.hotp.nbfail = 0;
                user->params.hotp.count += i + 1;
                updateOTPUser(user);
                if (unlockFile() != USR_SUCCESS) {
                    pam_syslog(pamh, LOG_ERR, "can't free lock on users");
                }
                pam_syslog(pamh, LOG_NOTICE, "%s logged in", user->username);
                return PAM_SUCCESS;
            }
        }
    } else {
        pam_info(pamh, "Veuillez ressayer dans %ld secondes.",
                 (user->params.hotp.tplstauth - time(NULL)));
        pam_syslog(pamh, LOG_ERR, "Nouvelle tentative trop rapide.");
        return PAM_AUTH_ERR;
    }
    user->params.hotp.tplstauth = time(NULL) + delay;
    user->params.hotp.nbfail += 1;
    updateOTPUser(user);

    pam_syslog(pamh, LOG_ERR,"%s failed to log in", user->username);

    if (unlockFile() != USR_SUCCESS) {
        pam_syslog(pamh, LOG_ERR, "can't free lock on users");
    }
    return PAM_AUTH_ERR;
}
/** Vérifie que qu'un utilisateur entre le bon OTP.
 *
 * Seule fonction qui devrait vraiment différer entre pam_hotp et pam_totp.
 */
int _check_otp(pam_handle_t * pamh, const char * username,
               const char * otp, modopt * options) {
    otpuser user;
    user.passwd = NULL;
    user.username = NULL;
    if (lockFile() != USR_SUCCESS) {
        pam_syslog(pamh, LOG_ERR, "can't get lock");
        return PAM_AUTH_ERR;
    }

    // Récupération des données utilisateurs.
    if (getOTPUser(username, &user) != USR_SUCCESS) {
        pam_syslog(pamh, LOG_ERR, "bad username %s", username);
        return PAM_USER_UNKNOWN;
    }
    int retval;
    switch(user.method) {
    case HOTP_METHOD:
        retval = _check_hotp(pamh, &user, otp, get_opt(options, DELAY_HOTP_AUTH));
        break;
    case TOTP_METHOD:
        retval = _check_totp(pamh, &user, otp, get_opt(options, DELAY_TOTP_AUTH));
        break;
    default:
        pam_syslog(pamh, LOG_ERR, "Unknown otp method");
        retval = PAM_AUTH_ERR;
        break;
    }
    resetOTPUser(&user);
    if (unlockFile() == -1) {
        pam_syslog(pamh, LOG_ERR, "can't free lock");
    }
    return retval;
}


int pam_sm_authenticate(pam_handle_t *pamh, int flags,
                        int argc, const char **argv) {

    const char * usrname;
    const char *otp2;
    char * otp;
    int retval;
    modopt  modstr;
    int authtok = PAM_AUTHTOK;

    // Récupération du nom d'utilisateur dans name.
    if ((retval = pam_get_user(pamh, &usrname, NULL)) != PAM_SUCCESS) {
        return retval;
    }
    if (usrname == NULL) {
        return PAM_USER_UNKNOWN;
    }

    // Obtention d'un OTP par PAM.
    if (parse_options(pamh, &modstr, argc, argv) == -1) {
        pam_syslog(pamh, LOG_ERR, "No options");
    }
    
    if (get_opt(&modstr, NULL_OK)) {
        if (userExists(usrname) == 0) {
            return PAM_SUCCESS;
        }
    }
        
    if (get_opt(&modstr, USE_AUTH_TOK)) {
        char * prev_auth_tok = NULL;
        if ((retval = pam_get_item(pamh, PAM_AUTHTOK,(void *) &prev_auth_tok))
            != PAM_SUCCESS) {
            return retval;
        }
        if ((retval = pam_set_item(pamh, PAM_AUTHTOK, NULL)) != PAM_SUCCESS) {
            return retval;
        }
        if (flags && PAM_PRELIM_CHECK) {
            authtok = PAM_OLDAUTHTOK;
        }
        if ((retval = pam_get_authtok(pamh, authtok,
                                      &otp2, "Mot de passe jetable: "))
                != PAM_SUCCESS) {
            return retval;
        }
        if (otp2 == NULL) {
            return PAM_AUTH_ERR;
        }

        /* Coutournement de la clause const on passe par un autre buffer
         * qui lui est modifiable.
         */
        int len = strlen(otp2);
        otp = malloc(sizeof(char) * (len + 1));
        if (otp == NULL) {
            return PAM_AUTH_ERR;
        }
        memcpy(otp, otp2, len * sizeof(char));
        otp[len] = 0;

        /* Un otp est par définition temporaire on le supprime donc du cache
         * de PAM pour qu'un autre module ne plante pas en récupérant un OTP.
         */
        pam_set_item(pamh, PAM_AUTHTOK, prev_auth_tok);
    } else {
        if ((retval = pam_prompt(pamh, PAM_PROMPT_ECHO_ON,
                                 &otp, "Mot de passe jetable: "))
                != PAM_SUCCESS) {
            return retval;
        }
    }

    retval = _check_otp(pamh, usrname, otp, &modstr);
    if (get_opt(&modstr, USE_AUTH_TOK)) {
        free(otp);
    }
    return retval;
}

/** Identique à pam_unix */
int pam_sm_setcred (pam_handle_t *pamh, int flags,
                    int argc, const char **argv) {
    int retval;
    const void *pretval = NULL;

    D(("called."));

    retval = PAM_SUCCESS;

    D(("recovering return code from auth call"));
    /* We will only find something here if UNIX_LIKE_AUTH is set --
     *      don't worry about an explicit check of argv. */
    if (pam_get_data(pamh, "unix_setcred_return", &pretval) == PAM_SUCCESS
            && pretval) {
        retval = *(const int *)pretval;
        pam_set_data(pamh, "unix_setcred_return", NULL, NULL);
        D(("recovered data indicates that old retval was %d", retval));
    }

    return retval;
}

int pam_sm_chauthtok (pam_handle_t *pamh, int flags,
                      int argc, const char **argv) {
    int retval;
    modopt options;

    if (parse_options(pamh, &options,argc, argv) == -1) {
        pam_syslog(pamh, LOG_ERR, "Options invalide détectées.");
    }

    // Obtenir le nom d'utilisateur.
    const char * username;
    if ((retval = pam_get_user (pamh, &username, NULL)) != PAM_SUCCESS) {
        return retval;
    }

    if (username == NULL) {
        pam_syslog (pamh, LOG_ERR, "Unknown user");
        return PAM_USER_UNKNOWN;
    }

    // 1er appel de la fonction avec le flag PAM_PRELIM_CHECK.
    // Test si l'utilisateur a un compte actif.
    if (flags & PAM_PRELIM_CHECK) {
        // Tout d'abord, qui éxecute le processus courant ?
        if (getuid () == 0) {
            // Si l'utilisateur est root, alors pas de conditions
            // supplémentaires, l'utilisateur à l'autorité
            return PAM_SUCCESS;
        }
        
        /* Si l'utilisateur appelant n'est pas l'utilisateur ciblé, la demande
         * de secret échouera.
         */
        size_t bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        struct passwd pwd;
        struct passwd * result;
        if (bufsize == -1) {
            bufsize = 16384;
        }
        char * buf = malloc(bufsize);
        if (buf == NULL) {
            return USR_ERR_SYS;
        }
        getpwnam_r(username, &pwd, buf, bufsize, &result);
        if (result == NULL) {
            free(buf);
            return USR_ERR_USR_UKN;
        }
        if (pwd.pw_uid != getuid()) {
            pam_info(pamh, "unable to ask a secret for %s", username);
            return PAM_TRY_AGAIN;
        }
        free(buf);

        switch (retval = userExists (username)) {
        case 0 :
            // Aucun compte existant, pré requis pour mettre à jour le
            // secret validé.
            return PAM_SUCCESS;
        case 1 :
            // Compte existant, il faudra vérifier s'il en est le
            // propriétaire
            break;
        default :
            // Une erreur s'est produite
            pam_syslog (pamh, LOG_ERR,
                        "error while accessing users file");
            return PAM_AUTHTOK_ERR;
        }

        // Vérifier l'autheticité du propriétaire par une demande
        // d'authentification (pas d'appel à pam_sm_authenticate pour ne pas
        // récupérer une deuxième fois le user)
        retval = pam_sm_authenticate(pamh, flags, argc, argv);
        if (retval != PAM_SUCCESS) {
            pam_syslog (pamh, LOG_ERR, "user authentication failed");
        }
        return PAM_TRY_AGAIN;
    }

    //  2eme appel de la fonction avec le flag PAM_UPDATE_AUTHTOK.
    if (flags & PAM_UPDATE_AUTHTOK) {
        otpuser user;
        user.passwd = NULL;
        user.username = NULL;
        char * retstr;
        struct pam_conv* conv;
        pam_get_item(pamh, PAM_CONV, (const void **) &conv);
        conv_data * appdata = (conv_data*) conv->appdata_ptr;
        /* On recréé l'utilisateur de zéro s'il existe déjà */

        // Nom
        user.username = strdup(username);

        // Demande de la méthode d'authentification
        if (appdata == NULL || appdata->method[0] == 0) {
            if ((retval = pam_prompt (pamh, PAM_PROMPT_ECHO_ON, &retstr,
                                      "Méthode d'authentification (hotp/totp) : ")) != PAM_SUCCESS) {
                resetOTPUser(&user);
                return retval;
            }
        } else {
            retstr = strndup(appdata->method, 4);
        }
        if (retstr == NULL) {
            pam_info(pamh, "Impossible de récupérer la méthode.");
            return PAM_PERM_DENIED;
        }
        if (strncasecmp (retstr, "totp", 5) == 0) {
            user.method = TOTP_METHOD;

            // Demande du quantum
            if (appdata == NULL || appdata->quantum == -1) {
                char * quantum;
                if ((retval = pam_prompt (pamh, PAM_PROMPT_ECHO_ON, &quantum,
                                          "Quantum : ")) != PAM_SUCCESS) {
                    free(retstr);
                    resetOTPUser(&user);
                    return retval;
                }
                if (quantum == NULL) {
                    pam_info(pamh, "Impossible de récupérer le quantum.");
                    return PAM_PERM_DENIED;
                }
                char * endptr;
                user.params.totp.quantum = strtol (quantum, &endptr, 10);
                if (*endptr != 0 || user.params.totp.quantum < 0) {
                    pam_info(pamh, "Quantum incorrect.");
                    free (quantum);
                    free(retstr);
                    resetOTPUser(&user);
                    return PAM_PERM_DENIED;
                }
                free (quantum);
            } else {
                user.params.totp.quantum = appdata->quantum;
                if (user.params.totp.quantum < 0) {
                    pam_info(pamh, "Quantum incorrect.");
                    free(retstr);
                    resetOTPUser(&user);
                    return PAM_PERM_DENIED;
                }
            }

            // Mise à 0 des pramètres optionnels pour TOTP
            user.params.totp.tps = 0;
            user.params.totp.delay = 0;
            user.params.totp.tplstauth = 0;

        } else if (strncasecmp (retstr, "hotp", 5) == 0) {
            user.method = HOTP_METHOD;
            user.params.hotp.count = 0;
            user.params.hotp.nbfail = 0;
            user.params.hotp.tplstauth = 0;
        } else {
            pam_info(pamh, "Méthode inconnue: %s", retstr);
            free(retstr);
            resetOTPUser(&user);
            return PAM_PERM_DENIED;
        }
        free(retstr);

        // Le secret
        user.passwd = createRandomSecret (16);
        if (user.passwd == NULL) {
            pam_info(pamh, "Impossible de générer un secret.");
            resetOTPUser(&user);
            return PAM_PERM_DENIED;
        }
        // Demande de la longueur des mots de passe générés
        if (appdata == NULL || appdata->length == -1) {
            if ((retval = pam_prompt (pamh, PAM_PROMPT_ECHO_ON, &retstr,
                                      "Taille des mots de passe : ")) != PAM_SUCCESS) {
                resetOTPUser(&user);
                return retval;
            }
            if (retstr == NULL) {
                pam_info(pamh, "Impossible de récupérer la taille des OTP.");
                return PAM_PERM_DENIED;
            }
            char* endptr;
            user.otp_len = (char) strtol (retstr, &endptr, 10);
            if (*endptr != 0) {
                pam_info(pamh, "Longueur OTP incorrect.");
                free (retstr);
                resetOTPUser(&user);
                return PAM_PERM_DENIED;
            }
            free (retstr);
        } else {
            user.otp_len = appdata->length;
        }
        if (user.otp_len < 6 || user.otp_len > 8) {
            pam_info(pamh, "Longueur OTP incorrect.");
            resetOTPUser(&user);
            return PAM_PERM_DENIED;
        }

        // État de l'utilisateur
        user.isBanned = 0;

        /* Mise à jour du fichier otpasswd */
        if (updateOTPUser (&user) != USR_SUCCESS) {
            pam_syslog (pamh, LOG_ERR, "user update failed");
            resetOTPUser(&user);
            return PAM_PERM_DENIED;
        }
        // Libération des ressources de l'utilisateur.
        // Prompt du nouveau secret
        int output_length = getLength(user.passwd) * 2 + 1;
        char output_secret[output_length];
        getHexRepresentation (user.passwd, output_secret, output_length);
        pam_info (pamh, "Le nouveau secret est : %s", output_secret);
        resetOTPUser(&user);

        return PAM_SUCCESS;
    }

    return PAM_PERM_DENIED;
}
