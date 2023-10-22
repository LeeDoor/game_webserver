#pragma once
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators

namespace user_data{
    using namespace boost::uuids;

    struct PlayerData{
        boost::uuids::uuid uuid;
        std::string login;
        std::string password;
    };
}