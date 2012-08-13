/* 
 * File:   PublicApiResource.h
 * Author: tsa
 *
 * Created on 8 août 2012, 14:35
 */

#include "includeFile.h"

#ifndef PUBLICAPIRESOURCE_H
#define	PUBLICAPIRESOURCE_H


class PublicApiResource : public Wt::WResource
{
    public:
        virtual ~PublicApiResource()
        {
            beingDeleted();
        }

    protected:
        std::string login;
        Wt::WString password;
        const Wt::Auth::AuthService *authService;
        Wt::Auth::PasswordService *passService;
        Wt::Auth::PasswordVerifier *verifier;
        bool authentified;
        
        
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
        {
            Session session(Utils::connection);
            authentified = false;
//            Session::configureAuth();
            
            this->authService = new Wt::Auth::AuthService();
            
            this->passService = new Wt::Auth::PasswordService(*this->authService);
            
            this->verifier = new Wt::Auth::PasswordVerifier();
            this->verifier->addHashFunction(new Wt::Auth::BCryptHashFunction(7));

            
            passService->setAttemptThrottlingEnabled(true);
            
            passService->setVerifier(this->verifier);
            
            
            
            
            if (!request.getParameterValues("login").empty())
            {
                this->login = request.getParameterValues("login")[0];
            }
            if (!request.getParameterValues("password").empty())
            {
                this->password = request.getParameterValues("password")[0];
            }
            
            const Wt::WString pass = this->password;
            const Wt::Auth::User *usr;
            // new transaction
            {
                try
                {
                    Wt::Dbo::Transaction transaction(session);
                    Wt::Dbo::ptr<AuthInfo::AuthIdentityType> authIdType = session.find<AuthInfo::AuthIdentityType>().where("\"identity\" = ?").bind(this->login);
                    if (Utils::checkId<AuthInfo::AuthIdentityType>(authIdType))
                    {
                        
                        const std::string userId = boost::lexical_cast<std::string>(authIdType.get()->authInfo().get()->user().id());
                        usr = new Wt::Auth::User(userId, UserDatabase(session));
                        
                        Wt::Auth::PasswordHash *hash = new Wt::Auth::PasswordHash(authIdType.get()->authInfo().get()->passwordMethod(), 
                                                                                 authIdType.get()->authInfo().get()->passwordSalt(),
                                                                                 authIdType.get()->authInfo().get()->passwordHash());
                        
                        if (this->passService->verifier()->verify(pass,*hash))
                        {
                            usr->setAuthenticated(true);
                            this->authentified = true;
                            Wt::log("info") << usr->id() << " logged.";
                        }
                        else
                        {
                            usr->setAuthenticated(false);
                            Wt::log("info") << usr->id() << " failure number : " << usr->failedLoginAttempts();
                        }
                    }
                    else
                    {
                        Wt::log("error") << "[PUBLIC API] User not found";
                        //TODO error behavior
                    }
                }
                catch(Wt::Dbo::Exception const& e)
                {
                    Wt::log("error") << e.what();
                    //TODO : behaviour in error case
                }
//                Wt::log("info") << myPasswordServiceResource.verifyPassword(*usr,pass);
            }

            
       
        }
};


#endif	/* PUBLICAPIRESOURCE_H */

