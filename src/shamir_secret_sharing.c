#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shamir.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage:\n");
        printf("  %s encrypt\n", argv[0]);
        printf("  %s decrypt <share1> <share2> ... <shareK>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "encrypt") == 0) {
        char secret[256];
        int n, k;

        printf("Enter secret to be encrypted: ");
        if (scanf("%255s", secret) != 1) {
            printf("Error reading secret\n");
            return 1;
        }
        printf("Enter total number of shares (n): ");
        if (scanf("%d", &n) != 1 || n < 2 || n > 255) {
            printf("Invalid number of shares (must be 2-255)\n");
            return 1;
        }
        printf("Enter minimum shares needed to reconstruct (k): ");
        if (scanf("%d", &k) != 1 || k < 2 || k > n) {
            printf("Invalid threshold (must be 2 <= k <= n)\n");
            return 1;
        }

        byte secret_len = strlen(secret);
        char *keys = shamir_gen_keys(secret, secret_len, (byte)n, (byte)k);

        printf("\nGenerated %d shares (any %d needed to reconstruct):\n", n, k);
        char *keys_copy = strdup(keys);
        char *key = strtok(keys_copy, " ");
        int idx = 1;
        while (key != NULL) {
            printf("  Share %d: %s\n", idx++, key);
            key = strtok(NULL, " ");
        }
        printf("\n");
        free(keys_copy);
        free(keys);

    } else if (strcmp(argv[1], "decrypt") == 0) {
        if (argc < 4) {
            printf("Error: provide at least 2 shares for decryption\n");
            printf("Usage: %s decrypt <share1> <share2> ... <shareK>\n", argv[0]);
            return 1;
        }

        int required_shares = argc - 2;
        int secret_size = strlen(argv[2]) / 2 - 1;
        if (secret_size <= 0) {
            printf("Error: invalid share format\n");
            return 1;
        }

        byte **shares = malloc(required_shares * sizeof(byte *));
        for (int i = 0; i < required_shares; ++i) {
            shares[i] = hex_to_bytes(argv[i + 2]);
        }

        byte *decrypted_secret = shamir_decrypt(shares, secret_size, required_shares);
        printf("Decrypted secret: %s\n", decrypted_secret);

        for (int i = 0; i < required_shares; ++i) {
            free(shares[i]);
        }
        free(shares);
        free(decrypted_secret);

    } else {
        printf("Unknown command: %s\n", argv[1]);
        printf("Use 'encrypt' or 'decrypt'\n");
        return 1;
    }

    return 0;
}