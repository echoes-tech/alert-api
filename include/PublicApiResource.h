/* 
 * File:   PublicApiResource.h
 * Author: tsa
 *
 * Created on 8 août 2012, 14:35
 */

#include "includeFile.h"

#ifndef PUBLICAPIRESOURCE_H
#define	PUBLICAPIRESOURCE_H

class PublicApiResource : public Wt::WResource {
public:

    virtual ~PublicApiResource() {
        beingDeleted();
    }

protected:
    std::string login;
    Wt::WString password;
//    const Wt::Auth::AuthService *authService;
//    Wt::Auth::PasswordService *passService;
//    Wt::Auth::PasswordVerifier *verifier;
    bool authentified;

    virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response) {
        Wt::log("info") << "[PUBLIC API] Identifying";
        // Setting the session
        Session session(Utils::connection);
        Session::configureAuth();
        
        // default : not authentified
        this->authentified = false;

        this->login = "";
        this->password = "";
        if (!request.getParameterValues("login").empty()) {
            this->login = request.getParameterValues("login")[0];
        }
        if (!request.getParameterValues("password").empty()) {
            this->password = request.getParameterValues("password")[0];
        }

        const Wt::WString pass = this->password;

        // transaction
        {
            try {
                Wt::Dbo::Transaction transaction(session);
                
                // we need to find the pass method, pass salt and pass hash from the database to verify the input password
                Wt::Dbo::ptr<AuthInfo::AuthIdentityType> authIdType = session.find<AuthInfo::AuthIdentityType > ().where("\"identity\" = ?").bind(this->login);
                if (Utils::checkId<AuthInfo::AuthIdentityType > (authIdType)) 
                {
                    // find the user from his login
                    Wt::Auth::User user = session.users().findWithIdentity(Wt::Auth::Identity::LoginName,this->login);
                    
                    if (!user.isValid()) 
                    {
                        Wt::log("info") << "[PUBLIC API] User invalid";
                        return;
                    }
                    
                    // set the password hash with the collected data
                    Wt::Auth::PasswordHash *hash = new Wt::Auth::PasswordHash(authIdType.get()->authInfo().get()->passwordMethod(),
                                                                                authIdType.get()->authInfo().get()->passwordSalt(),
                                                                                authIdType.get()->authInfo().get()->passwordHash());

                    // verify
                    if (session.passwordAuth().verifier()->verify(pass, *hash)) 
                    {
                        session.login().login(user);
                        user.setAuthenticated(true);
                        this->authentified = true;
                        Wt::log("info") << "[PUBLIC API] " << user.id() << " logged.";
                    } else 
                    {
                        user.setAuthenticated(false);
                        //TODO : make something with the number of failed attempts
                        Wt::log("info") << "[PUBLIC API] " << user.id() << " failure number : " << user.failedLoginAttempts();
                    }
                } else 
                {
                    Wt::log("error") << "[PUBLIC API] User not found";
                }
            } catch (Wt::Dbo::Exception const& e) 
            {
                Wt::log("error") << "[PUBLIC API] " << e.what();
            }
        }
    }
};


#endif	/* PUBLICAPIRESOURCE_H */

