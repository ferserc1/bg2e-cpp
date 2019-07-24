
#include <bg2db/buffer_load.hpp>

#include <fstream>

namespace bg2db {

    std::vector<char> loadBuffer(const bg2base::path & filePath) {
        std::ifstream file(filePath.toString(), std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::ios_base::failure("Could not load file buffer " + filePath.toString());
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();
        
        return buffer;
    }
}
