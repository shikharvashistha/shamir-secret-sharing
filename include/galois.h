#ifndef GALOIS_H
#define GALOIS_H

typedef unsigned char byte;

/* GF(2^8) arithmetic with irreducible polynomial x^8 + x^4 + x^3 + x + 1 */
byte addition(byte a, byte b);
byte time_x(byte a);
byte multiply(byte a, byte b);
byte power(byte a, byte b);
byte inverse(byte a);
byte divide(byte a, byte b);

#endif /* GALOIS_H */