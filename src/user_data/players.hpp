#pragma once
#include "player_data.hpp"
#include <vector>
#include <optional>
#include <string>

namespace user_data{
    class Players{
    public:
        std::optional<PlayerData> GetPlayerData(const uuid& uuid);

        bool RegisterPlayer(std::string&& login, std::string&& password);
    private:
        std::vector<PlayerData> players_;
    };
}