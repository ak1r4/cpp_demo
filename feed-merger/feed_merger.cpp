#include <string>
#include <vector>
#include <iostream>
#include "./utils.h"


std::vector<std::string> split_line(const std::string s, const char delimeter) {
    std::vector<std::string> splitted {};

    auto substr_begin = s.begin();
    auto ii = s.begin();

    for (; ii != s.end(); ++ii) {
        if ((*ii) == delimeter) {
            splitted.push_back(std::string(substr_begin, ii));
            substr_begin = ii + 1;
        }
    }
    splitted.push_back(std::string(substr_begin, ii));

    return splitted;
}
int main() {
    std::cout << split_line("a,b,c,d,e", ',') << "\n";
    std::cout << split_line("a,b,,d,e", ',') << "\n";
    std::cout << split_line("a,b,,d,", ',') << "\n";
    std::cout << split_line("12hjasdf,asdfadf,sdf8713,^fadf", ',') << "\n";
    std::cout << split_line("12hjasdf,asdfadf,sdf8713,^fadf", 'a') << "\n";

    return 0;
}
