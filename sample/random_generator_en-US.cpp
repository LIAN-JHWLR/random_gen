#include <iostream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <cstdint>
#include <immintrin.h>
#include <cpuid.h>

class IntelRandomGenerator {
private:
    bool rdrand_supported;
    bool rdseed_supported;

    // Check CPU support RDRAND instruction
    bool check_rdrand_support() {
        unsigned int eax, ebx, ecx, edx;
        __cpuid(1, eax, ebx, ecx, edx);
        return (ecx & (1 << 30)) != 0;
    }

    // Check CPU support RDSEED instruction
    bool check_rdseed_support() {
        unsigned int eax, ebx, ecx, edx;
        __cpuid_count(7, 0, eax, ebx, ecx, edx);
        return (ebx & (1 << 18)) != 0;
    }

public:
    IntelRandomGenerator() {
        rdrand_supported = check_rdrand_support();
        rdseed_supported = check_rdseed_support();

        std::cout << "RDRAND support: " << (rdrand_supported ? "yes" : "no") << std::endl;
        std::cout << "RDSEED support: " << (rdseed_supported ? "yes" : "no") << std::endl;
    }

    // Use RDRAND generate 32bit random number
    bool generate_rdrand32(uint32_t& value) {
        if (!rdrand_supported) return false;

        int retry = 10;
        while (retry-- > 0) {
            if (_rdrand32_step(&value)) {
                return true;
            }
        }
        return false;
    }

    // Use RDRAND generate 64bit random number
    bool generate_rdrand64(uint64_t& value) {
        if (!rdrand_supported) return false;

        int retry = 10;
        while (retry-- > 0) {
            if (_rdrand64_step(&value)) {
                return true;
            }
        }
        return false;
    }

    // Use RDRAND generate 32bit random number
    bool generate_rdseed32(uint32_t& value) {
        if (!rdseed_supported) return false;

        int retry = 10;
        while (retry-- > 0) {
            if (_rdseed32_step(&value)) {
                return true;
            }
        }
        return false;
    }

    // Use RDRAND generate 64bit random number
    bool generate_rdseed64(uint64_t& value) {
        if (!rdseed_supported) return false;

        int retry = 10;
        while (retry-- > 0) {
            if (_rdseed64_step(&value)) {
                return true;
            }
        }
        return false;
    }

    // Generate a random byte array of specified length
    std::vector<uint8_t> generate_random_bytes(size_t length, bool use_rdseed = false) {
        std::vector<uint8_t> result;
        result.reserve(length);

        while (result.size() < length) {
            uint64_t random_value;
            bool success;

            if (use_rdseed && rdseed_supported) {
                success = generate_rdseed64(random_value);
            } else if (rdrand_supported) {
                success = generate_rdrand64(random_value);
            } else {
                std::cerr << "Error: CPU does not support hardware random number generator" << std::endl;
                return result;
            }

            if (!success) {
                std::cerr << "Error: Random number generation failed" << std::endl;
                return result;
            }

            // Convert 64 bit random numbers to bytes
            for (int i = 0; i < 8 && result.size() < length; i++) {
                result.push_back((random_value >> (i * 8)) & 0xFF);
            }
        }

        return result;
    }

    // Generate a random string in hexadecimal format
    std::string generate_hex_string(size_t length, bool use_rdseed = false) {
        auto bytes = generate_random_bytes(length, use_rdseed);
        std::string hex_string;

        for (uint8_t byte : bytes) {
            std::stringstream ss;
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
            hex_string += ss.str();
        }

        return hex_string;
    }
};

int main() {
    IntelRandomGenerator generator;

    std::cout << "\n=== Intel CPU Hardware Random Number Generator ===" << std::endl;

    int choice;
    size_t length;

    while (true) {
        std::cout << "\nPlease select an operation:" << std::endl;
        std::cout << "1. Generate a random byte array (RDRAND)" << std::endl;
        std::cout << "2. Generate a random byte array (RDSEED)" << std::endl;
        std::cout << "3. Generate hexadecimal random string (RDRAND)" << std::endl;
        std::cout << "4. Generate hexadecimal random string (RDSEED)" << std::endl;
        std::cout << "5. Exit" << std::endl;
        std::cout << "Please enter your selection (1-5): ";

        std::cin >> choice;

        if (choice == 5) break;

        if (choice < 1 || choice > 4) {
            std::cout << "Invalid selection, please try again." << std::endl;
            continue;
        }

        std::cout << "Please enter the length (in bytes) to be generated: ";
        std::cin >> length;

        if (length <= 0 || length > 10000) {
            std::cout << "The length must be between 1-10000." << std::endl;
            continue;
        }

        switch (choice) {
            case 1: {
                auto bytes = generator.generate_random_bytes(length, false);
                std::cout << "Generated Random Byte (RDRAND): ";
                for (size_t i = 0; i < bytes.size(); i++) {
                    std::cout << std::hex << std::setw(2) << std::setfill('0')
                             << static_cast<int>(bytes[i]);
                    if ((i + 1) % 16 == 0) std::cout << std::endl;
                    else if ((i + 1) % 8 == 0) std::cout << " ";
                }
                std::cout << std::dec << std::endl;
                break;
            }
            case 2: {
                auto bytes = generator.generate_random_bytes(length, true);
                std::cout << "Generate random bytes (RDSEED): ";
                for (size_t i = 0; i < bytes.size(); i++) {
                    std::cout << std::hex << std::setw(2) << std::setfill('0')
                             << static_cast<int>(bytes[i]);
                    if ((i + 1) % 16 == 0) std::cout << std::endl;
                    else if ((i + 1) % 8 == 0) std::cout << " ";
                }
                std::cout << std::dec << std::endl;
                break;
            }
            case 3: {
                auto hex_string = generator.generate_hex_string(length, false);
                std::cout << "Generated hexadecimal string (RDRAND): " << hex_string << std::endl;
                break;
            }
            case 4: {
                auto hex_string = generator.generate_hex_string(length, true);
                std::cout << "Generated hexadecimal string (RDSEED): " << hex_string << std::endl;
                break;
            }
        }
    }

    return 0;
}
