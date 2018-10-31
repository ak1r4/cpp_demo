#include<vector>
#include<ostream>

template<typename T>
std::ostream& operator<< (std::ostream& os, const std::vector<T>& v) {
    os << "[";
    for (typename std::vector<T>::const_iterator ii = v.begin();
         ii != v.end();
         ++ii) {
        os << "\"" << *ii << "\"";

        if ((ii + 1) != v.end()) {
            os << ", ";
        }
    }
    os << "]";
    return os;
} 
