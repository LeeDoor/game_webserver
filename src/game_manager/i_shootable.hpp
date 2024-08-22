#pragma once
#include "event_manager.hpp"
#include <memory>

namespace game_manager{
    class Bullet;
    class IShootable{
    public:
        using Ptr = std::shared_ptr<IShootable>;
        using Arr = std::list<Ptr>;

        virtual EventListWrapper::Vec GetShot(int move_number, std::shared_ptr<Bullet> bullet) = 0;
    };
}