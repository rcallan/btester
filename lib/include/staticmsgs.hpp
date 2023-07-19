#include <iostream>

class staticmsgs
{
public:
    static std::string noinputfile;
    static std::string printargs;
};

std::string staticmsgs::noinputfile = "no input file was specified. input file is a mandatory argument";
std::string staticmsgs::printargs = "printing all arguments...";