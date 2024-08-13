#pragma once
#include "game_manager.hpp"
#include "thread_lock.hpp"
#include <map>
#include <optional>
#include <functional>
#include <memory>
#include <mutex>

namespace notification_system{
    class SessionStateNotifier{
    public:
        using Ptr = std::shared_ptr<SessionStateNotifier>;
        enum PollStatus{ // status of sent data
            Ok,
            NotRelevant,
            PollClosed
        };
        using Responser = std::function<void(PollStatus, std::optional<gm::EventListWrapper::CPtr>)>; // long poll responser
        using ResponserOpt = std::optional<Responser>; // optional responser
        using UsersResponser = std::map<um::Uuid, ResponserOpt>; // user's polls to response
        using PollWaiting = std::vector<um::Uuid>; // vector of users who didnt update poll
        struct SessionData{
            UsersResponser users_responser;
            PollWaiting poll_waiting;
        };
        using SessionSubs = std::map<gm::SessionId, SessionData>; // subscribers to session

        // SINGLETON
        SessionStateNotifier(SessionStateNotifier &other) = delete;
        void operator=(const SessionStateNotifier &) = delete;

        static Lock<SessionStateNotifier::Ptr> GetInstance();
        static void Init(gm::GameManager::Ptr gm);

        bool Subscribe(const um::Uuid& uuid, const gm::SessionId& sid);
        bool Unsubscribe(const um::Uuid& uuid, const gm::SessionId& sid);
        bool ChangePoll(const um::Uuid& uuid, const gm::SessionId& sid, Responser&& responser);
        bool Notify(const gm::SessionId& sid);  
        bool SessionCreated(const um::Uuid& player1, const um::Uuid& player2, const gm::SessionId& sid);
    private:
        SessionStateNotifier(gm::GameManager::Ptr gm);
        static SessionStateNotifier::Ptr pinstance_;
        static std::mutex mutex_;
        
        gm::EventListWrapper::OptCPtr GetEvents(const gm::SessionId& sid);

        SessionSubs sessions_;
        gm::GameManager::Ptr gm_;
    };
}