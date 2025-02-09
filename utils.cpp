#include "utils.h"

namespace Utils {
    // template <typename T>
    // int FindIndex(const std::vector<T>& container, const T& value) {
    //     auto it = std::find(container.begin(), container.end(), value);
    //     if (it != container.end())
    //         return it - container.begin();
    //     return -1;
    // }

    // template <typename T>
    // int FindIndex(const std::vector<T>& container, std::predicate condition)
    // {
    //     auto it = std::find_if(container.begin(), container.end(), condition);
    //     if (it != container.end())
    //         return it - container.begin();
        
    //     return -1;
    // }

    int FindClientIndexByName(const std::vector<std::shared_ptr<Client>>& clients, const std::string& name)
    {
        auto it = std::find_if(
            std::make_move_iterator(clients.begin()),
            std::make_move_iterator(clients.end()),
            [&name](const std::shared_ptr<Client>& client) { return client->name == name; });

        if (it != std::make_move_iterator(clients.end()))
            return it - std::make_move_iterator(clients.begin());
        
        return -1;
    }

    int FindTableIndex(const std::vector<std::unique_ptr<Table>>& tables, bool isBusy = false)
    {
        auto it = std::find_if(tables.begin(), tables.end(), 
            [&isBusy](const std::unique_ptr<Table>& table) { return table->isBusy == isBusy; });

        if (it != tables.end())
            return it - tables.begin();
        
        return -1;
    }
}
