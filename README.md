# Shamir's Secret Sharing

A C implementation of [Shamir's Secret Sharing](https://en.wikipedia.org/wiki/Shamir%27s_Secret_Sharing) scheme over GF(2^8) (Galois Field with 256 elements).

This scheme splits a secret into **n** shares such that any **k** shares can reconstruct the original secret, but fewer than **k** shares reveal **no information** about the secret whatsoever.

## How It Works

Shamir's Secret Sharing is based on polynomial interpolation over a finite field:

1. **Splitting**: For each byte of the secret, a random polynomial of degree `k-1` is created where the constant term equals the secret byte. The polynomial is evaluated at `n` distinct non-zero x-values to produce `n` shares.
2. **Reconstruction**: Given any `k` shares, [Lagrange interpolation](https://en.wikipedia.org/wiki/Lagrange_interpolation) recovers the polynomial and evaluates it at `x=0` to retrieve the original secret byte.

All arithmetic is performed in GF(2^8) using the irreducible polynomial `x^8 + x^4 + x^3 + x + 1` (the same field used in AES), which ensures:
- No integer overflow issues
- Addition = XOR (subtraction is the same as addition)
- Every non-zero element has a multiplicative inverse

## Project Structure

```
shamir-secret-sharing/
├── CMakeLists.txt              # Build configuration
├── include/
│   ├── galois.h                # GF(2^8) arithmetic declarations
│   └── shamir.h                # Shamir scheme declarations
├── src/
│   ├── galois.c                # GF(2^8) arithmetic implementation
│   ├── shamir.c                # Secret sharing implementation
│   └── shamir_secret_sharing.c # Main program (CLI)
└── bin/
    └── shamir_secret_sharing   # Compiled binary
```

## Prerequisites

- **GCC** (or any C99-compatible compiler)
- **CMake** >= 3.10

### Install on Ubuntu/Debian

```bash
sudo apt update
sudo apt install build-essential cmake
```

### Install on macOS

```bash
brew install cmake
```

## Building

```bash
# Clone the repository
git clone https://github.com/shikharvashistha/shamir-secret-sharing.git
cd shamir-secret-sharing

# Create build directory and compile
mkdir -p build && cd build
cmake ..
make

# The binary is output to bin/shamir_secret_sharing
cd ..
```

## Usage

### Encrypting (Splitting a Secret)

```bash
./bin/shamir_secret_sharing encrypt
```

You will be prompted for:
1. **Secret** — the text string to protect (up to 255 characters, no spaces)
2. **n** — total number of shares to generate (2–255)
3. **k** — minimum number of shares needed to reconstruct (2 ≤ k ≤ n)

**Example:**

```
$ ./bin/shamir_secret_sharing encrypt
Enter secret to be encrypted: hello
Enter total number of shares (n): 5
Enter minimum shares needed to reconstruct (k): 3

Generated 5 shares (any 3 needed to reconstruct):
  Share 1: 46c50010ba56
  Share 2: a543d3a581d8
  Share 3: 32348bd76367
  Share 4: 51cbc1c47cd8
  Share 5: f9c180d4292b
```

Each share is a hex string. The first byte (2 hex chars) is the x-coordinate, and the remaining bytes are the y-values for each byte of the secret.

### Decrypting (Reconstructing the Secret)

```bash
./bin/shamir_secret_sharing decrypt <share1> <share2> ... <shareK>
```

Provide at least **k** shares as command-line arguments.

**Example (using 3 of the 5 shares above):**

```
$ ./bin/shamir_secret_sharing decrypt 46c50010ba56 32348bd76367 51cbc1c47cd8
Decrypted secret: hello
```

Any combination of 3 or more shares from the set will recover the secret. Fewer shares produce meaningless output.

## Full Round-Trip Example

```bash
# Step 1: Split "mysecret" into 4 shares, requiring 3 to reconstruct
$ echo -e "mysecret\n4\n3" | ./bin/shamir_secret_sharing encrypt

Generated 4 shares (any 3 needed to reconstruct):
  Share 1: <hex1>
  Share 2: <hex2>
  Share 3: <hex3>
  Share 4: <hex4>

# Step 2: Reconstruct using any 3 shares
$ ./bin/shamir_secret_sharing decrypt <hex1> <hex3> <hex4>
Decrypted secret: mysecret
```

## Security Notes

- This implementation uses `rand()` seeded with `time()` for generating random polynomial coefficients and x-values. **This is not cryptographically secure** and is intended for educational purposes. For production use, replace with a CSPRNG (e.g., reading from `/dev/urandom`).
- The scheme is information-theoretically secure: fewer than `k` shares provide **zero** information about the secret, regardless of computational power.
- Share x-values are guaranteed to be unique and non-zero.
- The leading polynomial coefficient is guaranteed non-zero to ensure the full threshold is required.

## References

- [Shamir's Secret Sharing — Wikipedia](https://en.wikipedia.org/wiki/Shamir%27s_Secret_Sharing)
- [GF(2^8) Finite Field Arithmetic](https://en.wikipedia.org/wiki/Finite_field_arithmetic#Rijndael's_(AES)_finite_field)
- [libgfshare — Ubuntu Man Page](https://manpages.ubuntu.com/manpages/xenial/man7/gfshare.7.html)

## License

See [LICENSE](LICENSE) for details.