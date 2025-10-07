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
    
    // 检查CPU是否支持RDRAND指令
    bool check_rdrand_support() {
        unsigned int eax, ebx, ecx, edx;
        __cpuid(1, eax, ebx, ecx, edx);
        return (ecx & (1 << 30)) != 0;
    }
    
    // 检查CPU是否支持RDSEED指令
    bool check_rdseed_support() {
        unsigned int eax, ebx, ecx, edx;
        __cpuid_count(7, 0, eax, ebx, ecx, edx);
        return (ebx & (1 << 18)) != 0;
    }

public:
    IntelRandomGenerator() {
        rdrand_supported = check_rdrand_support();
        rdseed_supported = check_rdseed_support();
        
        std::cout << "RDRAND支持: " << (rdrand_supported ? "是" : "否") << std::endl;
        std::cout << "RDSEED支持: " << (rdseed_supported ? "是" : "否") << std::endl;
    }
    
    // 使用RDRAND生成32位随机数
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
    
    // 使用RDRAND生成64位随机数
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
    
    // 使用RDSEED生成32位随机数
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
    
    // 使用RDSEED生成64位随机数
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
    
    // 生成指定长度的随机字节数组
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
                std::cerr << "错误: CPU不支持硬件随机数生成器" << std::endl;
                return result;
            }
            
            if (!success) {
                std::cerr << "错误: 随机数生成失败" << std::endl;
                return result;
            }
            
            // 将64位随机数转换为字节
            for (int i = 0; i < 8 && result.size() < length; i++) {
                result.push_back((random_value >> (i * 8)) & 0xFF);
            }
        }
        
        return result;
    }
    
    // 生成十六进制格式的随机字符串
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
    
    std::cout << "\n=== Intel CPU 硬件随机数生成器 ===" << std::endl;
    
    int choice;
    size_t length;
    
    while (true) {
        std::cout << "\n请选择操作:" << std::endl;
        std::cout << "1. 生成随机字节数组 (RDRAND)" << std::endl;
        std::cout << "2. 生成随机字节数组 (RDSEED)" << std::endl;
        std::cout << "3. 生成十六进制随机字符串 (RDRAND)" << std::endl;
        std::cout << "4. 生成十六进制随机字符串 (RDSEED)" << std::endl;
        std::cout << "5. 退出" << std::endl;
        std::cout << "请输入选择 (1-5): ";
        
        std::cin >> choice;
        
        if (choice == 5) break;
        
        if (choice < 1 || choice > 4) {
            std::cout << "无效选择，请重试。" << std::endl;
            continue;
        }
        
        std::cout << "请输入要生成的长度 (字节数): ";
        std::cin >> length;
        
        if (length <= 0 || length > 10000) {
            std::cout << "长度必须在1-10000之间。" << std::endl;
            continue;
        }
        
        switch (choice) {
            case 1: {
                auto bytes = generator.generate_random_bytes(length, false);
                std::cout << "生成的随机字节 (RDRAND): ";
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
                std::cout << "生成的随机字节 (RDSEED): ";
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
                std::cout << "生成的十六进制字符串 (RDRAND): " << hex_string << std::endl;
                break;
            }
            case 4: {
                auto hex_string = generator.generate_hex_string(length, true);
                std::cout << "生成的十六进制字符串 (RDSEED): " << hex_string << std::endl;
                break;
            }
        }
    }
    
    return 0;
}