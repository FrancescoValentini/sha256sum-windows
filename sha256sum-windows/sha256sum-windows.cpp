#include <iostream>
#include <argparse/argparse.hpp>
#include "sha256sum-windows.h"
#include "Sha256Calculator.h"
#include <Windows.h>
#include <fstream> 

static void setupArguments(argparse::ArgumentParser& program) {
    // FILE (positional)
    program.add_argument("files")
        .help("Files to process, or '-' for standard input")
        .default_value(std::vector<std::string>{})
        .nargs(argparse::nargs_pattern::any);

    // Checksum verification mode
    program.add_argument("-c", "--check")
        .help("read checksums from the FILEs and check them")
        .default_value(false)
        .implicit_value(true);

    // Checksum verification options
    program.add_argument("--ignore-missing")
        .help("don't fail or report status for missing files")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("--quiet")
        .help("don't print OK for each successfully verified file")
        .default_value(false)
        .implicit_value(true);

    program.add_argument("--status")
        .help("don't output anything, status code shows success")
        .default_value(false)
        .implicit_value(true);
}

/// <summary>
/// Converts a Windows-style file path to a Unix-style path by replacing backslashes ('\') with slashes ('/').
/// This ensures compatibility with systems that use Unix style file paths.
/// </summary>
/// <param name="path">the path to convert</param>
/// <returns>A string with the converted Unix-style file path.</returns>
static std::string toUnixPath(const std::string& path) {
    std::string out = path;
    std::replace(out.begin(), out.end(), '\\', '/');
    return out;
}

/// <summary>
/// Prints the resulting SHA-256 hash in hexadecimal format and appends the file path
/// This function is used to output the hash and the file name in a readable format,
/// compatible with the sha256sum unix utility
/// </summary>
/// <param name="result">file cheksum</param>
/// <param name="name">file name</param>
static void formatOutput(const std::vector<BYTE>& result,
    const std::string& name) {
    for (BYTE byte : result)
        printf("%02x", byte);

    printf("  %s\n", toUnixPath(name).c_str());
}

/// <summary>
/// Calculates the SHA-256 hash for a given file or input stream.
/// After computation, it calls formatOutput to display the hash in the desired format.
/// </summary>
/// <param name="hInput">A file handle to the input file (can be standard input).</param>
/// <param name="name">The path of the file</param>
/// <returns>
/// EXIT_OK (0) if the operation succeeds.
/// EXIT_ERROR (2) if an exception is thrown during the calculation.
/// </returns>
static int sha256Calc(HANDLE hInput,
    const std::string& name,
    std::vector<BYTE>* outHash = nullptr) {
    try {
        sha256::Sha256Calculator hash;
        std::vector<BYTE> buffer(BUFFER_SIZE);

        DWORD bytesRead = 0;
        while (ReadFile(hInput, buffer.data(), BUFFER_SIZE, &bytesRead, nullptr) &&
            bytesRead > 0) {
            hash.update(buffer.data(), bytesRead);
        }

        const std::vector<BYTE>& result = hash.doFinal();

        if (outHash) {
            *outHash = result;
        }
        else {
            formatOutput(result, name);
        }

        return EXIT_OK;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return EXIT_ERROR;
    }
}

/// <summary>
/// Opens a file for reading. If the file is empty or "-" (standard input), it opens stdin instead.
/// </summary>
/// <param name="file">The path of the file to open, or "-" to indicate standard input.</param>
/// <returns>
/// A handle to the opened file (or stdin) on success, INVALID_HANDLE_VALUE if an error occurs.
/// </returns>
static HANDLE openInput(const std::string& file) {
    // STDIN
    if (file == "-" || file.empty()) {
        return GetStdHandle(STD_INPUT_HANDLE);
    }
    // File
    return CreateFileA(
        file.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN,
        nullptr
    );
}

/// <summary>
/// The main function that handles the processing of one or more files. 
/// </summary>
/// <param name="files">A vector of strings representing the file names to process.</param>
/// <returns>
/// EXIT_OK (0) if all files are processed successfully, 
/// EXIT_ERROR (2) if any error occurs during file processing or hashing.
/// </returns>
static int calculate(std::vector<std::string>& files) {
    int rc = 0;

    if (files.empty()) {
        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
        return sha256Calc(hInput, "-"); // Use '-' as the file name for stdin
    }

    for (const auto& file : files) {
        HANDLE hInput = openInput(file);

        if (hInput == INVALID_HANDLE_VALUE) {
            std::cerr << "Unable to open: " << file << "\n";
            return EXIT_ERROR;
        }

        rc = sha256Calc(hInput, file);

        if (rc != EXIT_OK) {
            return rc;
        }

        if (file != "-") {
            CloseHandle(hInput);
        }
    }

    return EXIT_OK;
}

/// <summary>
/// Converts a byte vector to a hexadecimal string
/// </summary>
/// <param name="hash">The byte vector</param>
/// <returns>Hexadecimal string</returns>
static std::string toHex(const std::vector<BYTE>& hash) {
    std::ostringstream oss;
    for (BYTE b : hash) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    }
    return oss.str();
}

/// <summary>
/// Parses the calculation output format and checks the file hashes
/// </summary>
/// <param name="files">Vector of files</param>
/// <param name="ignoreMissing">don't fail or report status for missing files</param>
/// <param name="quiet">don't print OK for each successfully verified file</param>
/// <param name="status">don't output anything, status code shows success</param>
/// <returns>
/// EXIT_OK (0) if all files are processed and verified successfully, 
/// EXIT_EXIT_MISMATCH (1) if At least one checksum does NOT match
/// EXIT_ERROR (2) if any error occurs during file processing or hashing.
/// </returns>
static int checkFiles(const std::vector<std::string>& files, bool ignoreMissing, bool quiet, bool status) {
    std::istream* in = &std::cin;
    std::ifstream file;

    bool allMatches = true;
    int errors = 0;
    int unredable = 0;

    if (!files.empty() && files[0] != "-") {
        file.open(files[0]);
        if (!file) {
            std::cerr << "Unable to open checksum file: " << files[0] << "\n";
            return EXIT_ERROR;
        }
        in = &file;
    }

    std::string line;
    int rc = EXIT_OK;

    while (std::getline(*in, line)) {
        if (line.empty())
            continue;

        std::istringstream iss(line);
        std::string expectedHex;
        std::string filename;

        iss >> expectedHex >> filename;

        if (expectedHex.empty() || filename.empty()) {
            std::cerr << "Invalid format: " << line << "\n";
            return EXIT_ERROR;
        }

        HANDLE hInput = openInput(filename);
        if (hInput == INVALID_HANDLE_VALUE) {
            allMatches = false;
            errors++;
            unredable++;
            if (!status && !ignoreMissing) std::cout << filename << ": FAILED\n";
            continue;
        }

        std::vector<BYTE> computed;
        int hashRc = sha256Calc(hInput, filename, &computed);

        if (filename != "-") {
            CloseHandle(hInput);
        }

        if (hashRc != EXIT_OK) {
            allMatches = false;
            errors++;
            if (!status) std::cout << filename << ": FAILED\n";
            continue;
        }

        if (toHex(computed) == expectedHex) {
            if(!quiet && !status) std::cout << filename << ": OK\n";
        }
        else {
            allMatches = false;
            errors++;
            if (!status) std::cout << filename << ": FAILED\n";
        }
    }

    if (errors>0) {
        if (!status) std::cerr<< "sha256sum-windows: WARNING: " << errors << " computed checksum did NOT match" << "\n";
        if (unredable > 0 && (!status && !ignoreMissing)) {
           std::cerr << "sha256sum-windows: WARNING: " << unredable << " listed file could not be read" << "\n";
        }
        return EXIT_MISMATCH;
    }

    return EXIT_OK;
}


int main(int argc, char* argv[]) {
    argparse::ArgumentParser program(PGM_NAME, PGM_VERS);
    setupArguments(program);

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << "\n";
        std::cerr << program;
        return EXIT_FAILURE;
    }

    bool check = program.get<bool>("--check");
    bool ignoreMissing = program.get<bool>("--ignore-missing");
    bool quiet = program.get<bool>("--quiet");
    bool status = program.get<bool>("--status");
    std::vector<std::string> files = program.get<std::vector<std::string>>("files");

    if (!check) {
        return calculate(files);
    }

    return checkFiles(files, ignoreMissing, quiet, status);
}
