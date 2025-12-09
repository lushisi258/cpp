#include <fstream>
#include <iostream>

int main() {
    std::ifstream jpg_input("./files/bronya.jpg", std::ios::binary);
    std::ofstream jpg_out("./target_dir/bronya_saved.jpg", std::ios::binary);
    if (jpg_input.is_open()) {
        char buf[4096];
        jpg_input.read(buf, sizeof(buf));
        while (jpg_input.gcount() >= sizeof(buf)){
            jpg_out.write(buf, sizeof(buf));
            jpg_input.read(buf, sizeof(buf));
        }
        jpg_out.write(buf, jpg_input.gcount());
        jpg_out.close();
        jpg_input.close();
    }
    return 0;
}