#pragma once
#include <boost/uuid/uuid.hpp>            // uuid class

namespace database_manager{
    namespace ids = boost::uuids;
    class IUserDataManager{
    public:
        virtual ids::uuid GenerateUuid() = 0;
        virtual bool AddLine() = 0;
    };
}