#pragma once
#include "api_handler.hpp"

namespace http_handler {
    class GeneralHandler : public ApiHandler,  public std::enable_shared_from_this<GeneralHandler> {
    public:
        using Ptr = std::shared_ptr<GeneralHandler>;

        GeneralHandler(HandlerParameters handler_parameters);

        void Init() override;
    private:
        // generate api function set from file
        void ApiFunctionsParse();
        
        // api functions
        void ApiRegister(SessionData rns);
        void ApiLogin(SessionData rns);
        void ApiGetProfileData(SessionData rns);
        
        user_manager::IUserManager::Ptr dm_;
        token_manager::ITokenManager::Ptr tm_;
    };

} // http_handler

