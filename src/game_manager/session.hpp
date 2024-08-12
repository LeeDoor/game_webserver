#pragma once
#include <string>
#include <mutex>
#include <map>
#include <variant>
#include "user.hpp"
#include "state.hpp"

namespace game_manager{
    using SessionId = std::string;
    class Session : public std::enable_shared_from_this<game_manager::Session>{
    public:
        using Ptr = std::shared_ptr<Session>;
        Session(um::Uuid player1, const um::Login& login1, um::Uuid player2, const um::Login& login2);

        /// @brief check if given player is in the session
        /// @param uuid given uuid 
        /// @return true if the player is in here
        bool HasPlayer(const um::Uuid& uuid);
        /// @brief get state of the session
        /// @return const pointer to state object
        State::CPtr GetState();

        /// @brief game result data about finished session
        struct Results{
            um::Uuid winner;
            um::Uuid loser;
        };

        /// @brief get session result data object if finished (std::nullopt overwise)
        std::optional<Results> GetResults();

        /// @brief type of player's move
        enum MoveType{
            Walk,
            Resign,
            PlaceBomb,
        };
        
        /// @brief player's move status
        enum GameApiStatus{
            Ok,
            WrongMove,
            NotYourMove,
        };

        /// @brief player's move data which have only placing info
        struct PlaceData{
            Dimention posX;
            Dimention posY;
        };

        /// @brief player's move data which have placing and direction info
        struct DirectedPlaceData{
            Direction direction;
            Dimention posX;
            Dimention posY;
        };

        using VariantData = std::variant<bool, PlaceData, DirectedPlaceData>;
        // player's api functions
        
        /// @brief walking api. moves player_id to place_data
        GameApiStatus ApiWalk(const um::Uuid& player_id, const PlaceData& place_data);
        /// @brief resign api. resign as player_id
        GameApiStatus ApiResign(const um::Uuid& player_id);
        /// @brief place bomb api. places player_id's bomb to place_data
        GameApiStatus ApiPlaceBomb(const um::Uuid& player_id, const PlaceData& place_data);
    private:
        /// @brief sets map with login and uuid for both players, sets default map settings like obstacles.
        /// doesnt matter who is first and who is second player passed. function declares who is moving first.
        /// @param login1 first player
        /// @param login2 second player
        void InitSessionState(const um::Login& login1, const um::Login& login2);
        /// @brief finishes session and sets value to results_.
        /// session ending should be handled by game_manager.
        /// @param firstWinner true if first player won.
        void FinishSession(bool firstWinner);
        /// @brief makes changes after each player's move. should be called at the end of API functions.
        void AfterMove();

        // object placement

        /// @brief places bomb on given place as passed player
        void PlaceBombObject(PlaceData place, Player::Login login);
        /// @brief removes given object from scene.
        void RemoveObject(Object::Ptr obj);

        // object acting
        
        /// @brief handles exploding bomb on given position
        void Explode(Dimention posX, Dimention posY);

        // assisting functions

        /// @brief returns true if given cell is valid to walk on or to place an object.
        bool ValidCell(unsigned posX, unsigned posY);

        std::mutex move_mutex_;
        State::Ptr state_;

        um::Uuid player1_;
        um::Uuid player2_;

        std::optional<Results> results_;

        State::Objects& objects(){return state_->objects;}
        Player& player1(){return state_->players[0];}
        Player& player2(){return state_->players[1];}
        um::Login& nowTurn(){return state_->now_turn;}
        State::Terrain& terrain(){return state_->terrain;}

        const std::map<um::Uuid, um::Login> uuid_to_login_;
    };
}   
namespace gm = game_manager;