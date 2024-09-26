#include "i_placeable.hpp"

namespace game_manager {
    class Obstacle : public IPlaceable {
    public:
        using Ptr = std::shared_ptr<Obstacle>;

        enum Type{
            Wall,
        };

        Obstacle(){}
        Obstacle(Position position, Type type, ActorId actor_id = 0)
            :IPlaceable(actor_id),type(type){
            Place(position);
        }

        bool operator == (const Obstacle& other) const = default;

        Type type = Type::Wall;
    };
}