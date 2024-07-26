#pragma once
#include "session.hpp"
#include <map>
#include <optional>
#include <memory>
#include <mutex>

namespace notification_system{
    class SessionStateNotifier{
    public:
        using Ptr = std::shared_ptr<SessionStateNotifier>;
        enum PollStatus{ // status of sent data
            Ok,
            PollClosed
        };
        using Responser = std::function<void(PollStatus, gm::State::OptCPtr)>; // long poll responser
        using ResponserOpt = std::optional<Responser>; // optional responser
        using UsersResponser = std::map<dm::Uuid, ResponserOpt>; // user's polls to response
        using PollWaiting = std::vector<dm::Uuid>; // vector of users who didnt update poll
        struct SessionData{
            UsersResponser users_responser;
            PollWaiting poll_waiting;
        };
        using SessionSubs = std::map<gm::SessionId, SessionData>; // subscribers to session

        // SINGLETON
        SessionStateNotifier(SessionStateNotifier &other) = delete;
        void operator=(const SessionStateNotifier &) = delete;

        static SessionStateNotifier::Ptr GetInstance();
        static void Init(gm::GameManager::Ptr gm);
        //

        bool Subscribe(const dm::Uuid& uuid, const gm::SessionId& sid);
        bool Unsubscribe(const dm::Uuid& uuid, const gm::SessionId& sid);
        bool ChangePoll(const dm::Uuid& uuid, const gm::SessionId& sid, Responser&& responser);
        bool Notify(const gm::SessionId& sid);  
        bool SessionCreated(const dm::Uuid& player1, const dm::Uuid& player2, const gm::SessionId& sid);
    private:
        SessionStateNotifier(gm::GameManager::Ptr gm);
        static SessionStateNotifier::Ptr pinstance_;
        static std::mutex mutex_;
        
        gm::State::OptCPtr GetGameState(const gm::SessionId& sid);

        SessionSubs sessions_;
        gm::GameManager::Ptr gm_;
    };
}