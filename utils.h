#pragma once

#include <vector>
#include <algorithm>
#include "common/types.h"
// #include <concepts>

namespace Utils {
    // template <typename T>
    // int FindIndex(const std::vector<T>& container, const T& value);

    // template <typename T>
    // int FindIndex(const std::vector<T>& container, std::predicate condition);

    int FindClientIndexByName(const std::vector<Client*>& clients, const std::string& name);
    int FindTableIndex(const std::vector<Table*>& tables, bool isBusy);
}