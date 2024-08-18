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

        /// @brief subscribes given user to given session's updates
        /// @param uuid given user
        /// @param sid given session
        /// @return true if subscribed. false if already subscribed
        bool Subscribe(const um::Uuid& uuid, const gm::SessionId& sid);
        /// @brief unsubscribes everyone in this session:
        /// sends POLL_CLOSED to everyone who is waiting for poll. moves to special
        /// queue dudes who didnt send request yet, but who needs response. 
        /// @param sid this session
        bool Unsubscribe(const gm::SessionId& sid);
        /// @brief sets responser to poll. server uses responser to send data back.
        /// @param uuid poll's player
        /// @param sid session id
        /// @param responser function to response to player back.
        /// @return returns true if success. false if no such session.
        bool ChangePoll(const um::Uuid& uuid, const gm::SessionId& sid, Responser&& responser);
        /// @brief notifies all subscribed players with event list and adds to poll_waiting
        /// who didnt change poll yet.
        /// @param sid 
        /// @return true
        bool Notify(const gm::SessionId& sid);  
        /// @brief called when session created. subscribes both players to get event data.
        /// @param player1 
        /// @param player2 
        /// @param sid session id to subscribe.
        /// @return true
        bool SessionCreated(const um::Uuid& player1, const um::Uuid& player2, const gm::SessionId& sid);
    private:
        SessionStateNotifier(gm::GameManager::Ptr gm);
        static SessionStateNotifier::Ptr pinstance_;
        static std::mutex mutex_;
        
        /// @brief gets event list from game_manager with given sid
        /// @param sid given session id
        /// @return optional pointer to constant event list wrapper
        gm::EventListWrapper::OptCPtr GetEvents(const gm::SessionId& sid);

        /// @brief to each session id there is corresponding poll_waiting list and users_responser.
        /// poll_waiting - dudes who skipped notification. when they replace the poll, they instantly
        /// get response.
        /// users_responser - structure to keep users' responser functions. if responser is 
        /// nullopt, it means user didnt change poll yet.
        SessionSubs sessions_;
        /// @brief map of players, who didnt get fresh data yet, but session is closed and removed.
        std::map<std::pair<gm::SessionId, um::Uuid>, gm::EventListWrapper::OptCPtr> send_and_close_;
        gm::GameManager::Ptr gm_;
    };
}