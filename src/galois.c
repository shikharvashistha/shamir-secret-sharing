#include "galois.h"

byte addition(byte a, byte b) {
    return a ^ b;
}

byte time_x(byte a) {
    /* Multiply by x in GF(2^8) with reduction polynomial x^8+x^4+x^3+x+1 (0x1B) */
    return (byte)((a << 1) ^ ((a & 0x80) ? 0x1B : 0));
}

byte multiply(byte a, byte b) {
    byte result = 0;
    for (int i = 0; i < 8; ++i) {
        if (b & (1 << i)) {
            result ^= a;
        }
        a = time_x(a);
    }
    return result;
}

byte power(byte a, byte b) {
    byte result = 1;
    for (int i = 0; i < 8; ++i) {
        if (b & (1 << i)) {
            result = multiply(result, a);
        }
        a = multiply(a, a);  /* Square for next bit */
    }
    return result;
}

byte inverse(byte a) {
    if (a == 0) return 0;
    /* a^254 = a^{-1} in GF(2^8) since the multiplicative group has order 255 */
    return power(a, 0xFE);
}

byte divide(byte a, byte b) {
    return multiply(a, inverse(b));
}
