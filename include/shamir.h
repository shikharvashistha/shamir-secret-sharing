#ifndef SHAMIR_H
#define SHAMIR_H

#include "galois.h"

char *bytes_to_hex(byte *bytes, int len);
byte *hex_to_bytes(const char *hex);
byte *random_x_values(byte total_shares);
byte *shamir_decrypt(byte **shares, int secret_size, int required_shares);
char *shamir_gen_keys(char *secret, byte secret_size, byte total_shares, byte required_shares);
byte **shamir_split(byte secret_number, byte *x_arr, byte required_shares, byte total_shares);
byte **coefficient_polynomial(const byte *x_arr, byte secret_number, byte required_shares, byte total_shares);
byte *coefficients(byte secret_number, byte required_shares);
byte interpolate(byte *x_decryp, byte *y_decryp, int k);

#endif /* SHAMIR_H */
