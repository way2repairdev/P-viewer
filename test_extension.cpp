#include <iostream>
#include <string>

// Simplified version of the file extension check logic
std::string getFileExtension(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    if (pos != std::string::npos) {
        std::string ext = filename.substr(pos + 1);
        // Convert to lowercase for comparison
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext;
    }
    return "";
}

bool isValidFileExtension(const std::string& filename) {
    std::string ext = getFileExtension(filename);
    // This is the fixed logic from main.cpp
    return !(ext != "xzz" && ext != "pcb" && ext != "xzzpcb");
}

int main() {
    // Test the file extension logic
    std::cout << "Testing file extension logic after fix:\n";
    
    std::cout << "test.xzz: " << (isValidFileExtension("test.xzz") ? "VALID" : "INVALID") << std::endl;
    std::cout << "test.pcb: " << (isValidFileExtension("test.pcb") ? "VALID" : "INVALID") << std::endl;
    std::cout << "test.xzzpcb: " << (isValidFileExtension("test.xzzpcb") ? "VALID" : "INVALID") << std::endl;
    std::cout << "test.txt: " << (isValidFileExtension("test.txt") ? "VALID" : "INVALID") << std::endl;
    
    return 0;
}
