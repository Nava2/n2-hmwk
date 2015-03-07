

/**
 * @brief The CLIInput struct holds information parsed from the command line
 */
typedef struct _CLIInput {
    const std::string first;
    const std::string second;
    const size_t n;
    const bool shouldPrint;
} CLIInput;

std::ostream& operator<<(std::ostream& os, const CLIInput& obj)
{
    // write obj to stream
    os << "CLIInput{first=" << obj.first
       << ", second=" << obj.second
       << ", n=" << obj.n
       << ", shouldPrint=" << std::boolalpha << obj.shouldPrint << "}";

    return os;
}


int main(const int argc, const char** argv) {

    if (argc != 5) {
        std::cerr << "Invalid arguments passed. " << std::endl;
        std::cerr << "Usage: cs4442b-asn2-p2 <firstInputText:string> <secondInputText:string> <nGramCount:int>" << std::endl;

        return 1;
    }

    const CLIInput input = { argv[1], argv[2], std::stoul(argv[3]), std::stoul(argv[4]) > 0 };

    std::cout << input << std::endl;

    const kbright2::Database<std::string> first(input.n, input.first), second(input.n, input.second);

    computeDifference(first, second, input.shouldPrint);

    return 0;
}

