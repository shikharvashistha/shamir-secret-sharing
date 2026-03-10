#include "shamir.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char *bytes_to_hex(byte *bytes, int len) {
    char *hex = malloc(len * 2 + 1);
    for (int i = 0; i < len; ++i) {
        sprintf(hex + i * 2, "%02x", bytes[i]);
    }
    hex[len * 2] = '\0';
    return hex;
}

byte *hex_to_bytes(const char *hex) {
    int len = strlen(hex) / 2;
    byte *bytes = malloc(len);
    for (int i = 0; i < len; ++i) {
        char tmp[3] = {hex[i * 2], hex[i * 2 + 1], '\0'};
        bytes[i] = (byte)strtol(tmp, NULL, 16);
    }
    return bytes;
}

byte *random_x_values(byte total_shares) {
    byte *x = malloc(total_shares);
    for (int i = 0; i < total_shares; ++i) {
        byte val;
        int unique;
        do {
            val = (rand() % 255) + 1;  /* Range [1,255], x must never be 0 */
            unique = 1;
            for (int j = 0; j < i; ++j) {
                if (x[j] == val) {
                    unique = 0;
                    break;
                }
            }
        } while (!unique);
        x[i] = val;
    }
    return x;
}

byte *coefficients(byte secret_number, byte required_shares) {
    byte *matrix = malloc(required_shares * sizeof(byte));
    /* Random coefficients for higher-degree terms */
    for (int i = 0; i < required_shares - 1; ++i) {
        matrix[i] = rand() % 256;
        /* Leading coefficient (i==0) must be non-zero for security */
        if (i == 0 && required_shares > 1 && matrix[i] == 0) {
            matrix[i] = (rand() % 255) + 1;
        }
    }
    /* Constant term = secret (polynomial evaluated at x=0 gives the secret) */
    matrix[required_shares - 1] = secret_number;
    return matrix;
}

byte **coefficient_polynomial(const byte *x_arr, byte secret_number, byte required_shares, byte total_shares) {
    byte **shares = malloc(total_shares * sizeof(byte *));
    byte *coeff = coefficients(secret_number, required_shares);

    for (int i = 0; i < total_shares; ++i) {
        shares[i] = malloc(2 * sizeof(byte));
        byte x = x_arr[i];
        byte y = 0;
        /* Evaluate polynomial: coeff[0]*x^(k-1) + coeff[1]*x^(k-2) + ... + coeff[k-1] */
        for (int j = 0; j < required_shares; ++j) {
            y = addition(y, multiply(coeff[j], power(x, required_shares - j - 1)));
        }
        shares[i][0] = x;
        shares[i][1] = y;
    }
    free(coeff);
    return shares;
}

byte **shamir_split(byte secret_number, byte *x_arr, byte required_shares, byte total_shares) {
    return coefficient_polynomial(x_arr, secret_number, required_shares, total_shares);
}

byte interpolate(byte *x_decryp, byte *y_decryp, int k) {
    /* Lagrange interpolation at x=0 in GF(2^8) */
    byte y = 0;
    for (int j = 0; j < k; ++j) {
        byte prod = 0x01;
        for (int i = 0; i < k; ++i) {
            if (i != j) {
                /* L_j(0) = prod_{i!=j} (x_i / (x_i + x_j)) */
                prod = multiply(prod, divide(x_decryp[i], addition(x_decryp[i], x_decryp[j])));
            }
        }
        y = addition(y, multiply(prod, y_decryp[j]));
    }
    return y;
}

char *shamir_gen_keys(char *secret, byte secret_size, byte total_shares, byte required_shares) {
    srand(time(NULL));
    byte *x_arr = random_x_values(total_shares);

    /* Each key: 1 byte x-coord + secret_size bytes of y-values */
    int key_bytes = 1 + secret_size;
    int key_hex_len = key_bytes * 2;
    /* Output: space-separated hex strings + null terminator */
    char *keys = calloc(total_shares * (key_hex_len + 1) + 1, 1);

    /* combined[share_idx] = [x, y0, y1, ..., y_{n-1}] */
    byte **combined = malloc(total_shares * sizeof(byte *));
    for (int i = 0; i < total_shares; ++i) {
        combined[i] = calloc(key_bytes, 1);
        combined[i][0] = x_arr[i];
    }

    /* Split each byte of the secret independently */
    for (int i = 0; i < secret_size; ++i) {
        byte **shares = shamir_split((byte)secret[i], x_arr, required_shares, total_shares);
        for (int j = 0; j < total_shares; ++j) {
            combined[j][i + 1] = shares[j][1];
            free(shares[j]);
        }
        free(shares);
    }

    /* Format output as hex strings */
    for (int i = 0; i < total_shares; ++i) {
        char *hex = bytes_to_hex(combined[i], key_bytes);
        strcat(keys, hex);
        if (i < total_shares - 1) strcat(keys, " ");
        free(hex);
        free(combined[i]);
    }
    free(combined);
    free(x_arr);
    return keys;
}

byte *shamir_decrypt(byte **shares, int secret_size, int required_shares) {
    byte *secret = malloc((secret_size + 1) * sizeof(byte));

    for (int i = 0; i < secret_size; ++i) {
        byte *x_dec = malloc(required_shares * sizeof(byte));
        byte *y_dec = malloc(required_shares * sizeof(byte));
        for (int j = 0; j < required_shares; ++j) {
            x_dec[j] = shares[j][0];
            y_dec[j] = shares[j][i + 1];
        }
        secret[i] = interpolate(x_dec, y_dec, required_shares);
        free(x_dec);
        free(y_dec);
    }
    secret[secret_size] = '\0';
    return secret;
}
