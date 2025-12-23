# sha256sum-windows
[🇮🇹 Italian version](README-it.md)
## 📖 General Description

**sha256sum-windows** is a **Windows command-line utility** designed to calculate the **SHA-256** hash of one or more files or of data provided via **stdin**.

This project has **no relation to GNU coreutils**.
The code is a **from-scratch implementation**, using **only the cryptographic APIs provided by the Windows API (WinAPI)**.

It is intended to run **exclusively on Windows on x86 and amd64 platforms**, and **no ports to other operating systems are planned**.

## ✨ Features

- SHA-256 hash calculation using **WinAPI Crypto**, with no external cryptographic library dependencies
- **Output format compatible** with `sha256sum` on Unix/Linux systems
- Support for both **files** and **stdin**
- Processing of **one or multiple files** in a single invocation
- Suitable for use in pipelines and automated scripts

## 🚀 Usage

Common usage examples:

```bash
# Hash calculation
sha256sum-windows file.txt

sha256sum-windows file1.bin file2.bin

type file.txt | sha256sum-windows

sha256sum-windows file.txt > checksum.sha256

# Hash verification
sha256sum-windows -c checksum.sha256
```

## 🧾 Output Format

The output format is **compatible** with that produced by `sha256sum` on Unix systems:

```
<sha256_hash>  <file>

336d4f9173bd4797e001b13e269f5fb2154715b5ee22f0260448aae6aab72149  ./file.pdf
```

When the input comes from **stdin**, the file name is replaced with `-`.

## 📦 External Dependencies

* [https://github.com/p-ranav/argparse](https://github.com/p-ranav/argparse)

## ⚠️ Disclaimer

The author **assumes no responsibility** for any direct or indirect damage resulting from the use of this software, including data loss, illegal use, unexpected behavior, or incorrect results.
