#pragma once

#define PGM_NAME "sha256sum-windows"
#define PGM_VERS "1.0.0"

// Exit codes
#define EXIT_OK        0  // All checksums OK / operation successful
#define EXIT_MISMATCH  1  // At least one checksum does NOT match
#define EXIT_ERROR     2  // Error (missing file, invalid format, IO, etc.)

#define BUFFER_SIZE    16384 // bytes