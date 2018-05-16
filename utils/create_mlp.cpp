#include "nn.hpp"
#include <sstream>

int main(int argc, char** argv)
{
    std::stringstream ss;
    std::vector<size_t> sizes;

    for (int i = 1; i < argc; i++) {
        ss << argv[i];

        size_t s;
        ss >> s;
        ss.clear();

        sizes.push_back(s);
    }

    MLP mlp(sizes);

    std::cout << mlp;

    return 0;
}
