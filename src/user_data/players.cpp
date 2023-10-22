#include "players.hpp"
#include <algorithm>

namespace user_data{
    std::optional<PlayerData> Players::GetPlayerData(const uuid& uuid){
        auto iter = std::find_if(players_.begin(), players_.end(), [&](const PlayerData& pd){return pd.uuid == uuid;});
        if(iter == players_.end())
            return std::nullopt;
        return *iter;
    }

    bool Players::RegisterPlayer(std::string&& login, std::string&& password) {
        if(std::find_if(players_.begin(), players_.end(), [&](const PlayerData& pd){return pd.login == login;}) == players_.end()) {
            players_.push_back({random_generator()(), std::move(login), std::move(password)});
            return true;
        }
        return false;
    }
}