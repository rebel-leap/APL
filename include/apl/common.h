#pragma once
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <functional>

namespace apl {
    using String = std::string;
    template<typename T> using Vector = std::vector<T>;
    template<typename K, typename V> using Map = std::unordered_map<K, V>;
}