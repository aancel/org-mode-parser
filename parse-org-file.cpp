#include "OrgFile.hpp"

int main(int argc, char ** argv)
{
    if(argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <file.org>" << std::endl;
        exit(1);
    }

    org::File org_file(argv[1]);
    std::cout << org_file.to_string() << std::endl;
}
