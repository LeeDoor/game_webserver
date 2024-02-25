#pragma once
#include <string>
#include <vector>
#include <memory>
namespace matchmaking_system {
    class MMQueue{
        using Uuid = std::string;
    public:
        using Ptr = std::shared_ptr<MMQueue>;

        bool EnqueuePlayer(const Uuid& uuid);
        bool DequeuePlayer(const Uuid& uuid);

    private:
        void QueueUpdate();

        std::vector<Uuid> queue_;
    };
}