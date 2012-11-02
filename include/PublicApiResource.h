/* 
 * Header of API Ressource
 * @author ECHOES Technologies (TSA)
 * @date 08/08/2012
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2012 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef PUBLICAPIRESOURCE_H
#define	PUBLICAPIRESOURCE_H

#include "includeFile.h"

class PublicApiResource : public Wt::WResource {
    public:
    virtual ~PublicApiResource() {
        beingDeleted();
    }

    protected:
        std::string login;
        Wt::WString password;
        bool authentified;
        Session *session;

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response) {
            Wt::log("info") << "[PUBLIC API] Identifying";
            // Setting the session
            session = new Session(Utils::connection);
            Session::configureAuth();

            try 
            {
                session->createTables();
                std::cerr << "Created database." << std::endl;
            } catch (std::exception& e) {
                std::cerr << e.what() << std::endl;
                std::cerr << "Using existing database";
            }

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
                    Wt::Dbo::Transaction transaction(*session);

                    // check whether the user exists
                    Wt::Dbo::ptr<AuthInfo::AuthIdentityType> authIdType = session->find<AuthInfo::AuthIdentityType > ().where("\"identity\" = ?").bind(this->login);
                    if (Utils::checkId<AuthInfo::AuthIdentityType > (authIdType)) 
                    {
                        // find the user from his login
                        Wt::Auth::User user = session->users().findWithIdentity(Wt::Auth::Identity::LoginName,this->login);

                        if (!user.isValid()) 
                        {
                            Wt::log("info") << "[PUBLIC API] User invalid";
                            return;
                        }

                        // verify
                        switch (session->passwordAuth().verifyPassword(user, pass))
                        {
                            case Wt::Auth::PasswordValid:
                                session->login().login(user);
                                this->authentified = true;
                                Wt::log("info") << "[PUBLIC API] " << user.id() << " logged.";
                                break;
                            case Wt::Auth::LoginThrottling:
                                Wt::log("info") << "[PUBLIC API] too many attempts.";
                                break;
                            case Wt::Auth::PasswordInvalid:
                                Wt::log("info") << "[PUBLIC API] " << user.id() << " failure number : " << user.failedLoginAttempts();
                                break;
                            default:
                                break;
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

        std::string request2string(Wt::Http::Request r) {
            char c;
            std::string s;

            // Getting the input stream for the request char by char
            c = r.in().get();
            while (r.in())
            {
                s.append(1,c);
                c = r.in().get();
            }
            return s;
        }
};


#endif	/* PUBLICAPIRESOURCE_H */

