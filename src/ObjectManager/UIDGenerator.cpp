#include "UIDGenerator.h"

#include <random>
#include <array>
#include <sstream>
#include <iomanip>

// This will generate an UUID v4
std::string UIDGenerator::generateUID()
{
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);

    // UID is 16 bytes
    std::array<uint8_t, 16> uuid_bytes;

    // Fill with random bytes
    for (auto& byte : uuid_bytes) {
        byte = static_cast<uint8_t>(dist(gen) & 0xFF);
    }

    // Set version (4) and variant (RFC 4122)
    uuid_bytes[6] = (uuid_bytes[6] & 0x0F) | 0x40; // Version 4
    uuid_bytes[8] = (uuid_bytes[8] & 0x3F) | 0x80; // Variant 1

    // Convert to string format: 8-4-4-4-12
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (size_t i = 0; i < uuid_bytes.size(); ++i) {
        oss << std::setw(2) << static_cast<int>(uuid_bytes[i]);
        if (i == 3 || i == 5 || i == 7 || i == 9) {
            oss << "-";
        }
    }

    return oss.str();
}