/* 
 * Itooki SMS Acknowledgement Receiver
 * @author ECHOES Technologies (TSA)
 * @date 08/08/2012
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2012-2013 BY ECHOES TECHNOLGIES SAS
 * 
 */
#include "itooki/ItookiAckReceiver.h"

using namespace std;

ItookiAckReceiver::ItookiAckReceiver(Echoes::Dbo::Session& session): PublicItookiResource::PublicItookiResource(session)
{
    
}

ItookiAckReceiver::~ItookiAckReceiver()
{
}


EReturnCode ItookiAckReceiver::postAck(map<string, long long> parameters, const vector<string> &pathElements, const string &sRequest, string &responseMsg)
{
    const Wt::WDateTime now = Wt::WDateTime::currentDateTime();
    
    Wt::Dbo::Transaction transaction(m_session);
    
    EReturnCode res = EReturnCode::OK;
    
    bool    sended = false;
    Wt::WString  error = "missing";
    Wt::WString  refenvoi = "missing";
    
    if (!sRequest.empty())
        {
            try
            {
                Wt::Json::Object result;
                Wt::Json::parse(sRequest, result);

                if (result.contains("refenvoi"))
                {
                    refenvoi = result.get("refenvoi");
                }
                if (result.contains("error"))
                {
                    error = result.get("error");
                }
                if (result.contains("sended"))
                {
                    Wt::WString recu = result.get("sended");                 
                    sended = (recu == "true");
                }              


            }
            catch (Wt::Json::ParseError const& e)
            {
                res = EReturnCode::BAD_REQUEST;
                responseMsg = httpCodeToJSON(res, e);
            }
            catch (Wt::Json::TypeException const& e)
            {
                res = EReturnCode::BAD_REQUEST;
                responseMsg = httpCodeToJSON(res, e);
            }
            if(sended)
            {
                Wt::Dbo::ptr<Echoes::Dbo::Message> msgPtr = m_session.find<Echoes::Dbo::Message>()
                        .where(QUOTE(TRIGRAM_MESSAGE SEP "REF" SEP "ACK") " = ?").bind(refenvoi)
                        .where(QUOTE(TRIGRAM_MESSAGE SEP "DELETE") " IS NULL");
                
                if(msgPtr)
                {
                    Echoes::Dbo::MessageTrackingEvent *newStateMsg = new Echoes::Dbo::MessageTrackingEvent();

                    newStateMsg->date = now;
                    newStateMsg->message = msgPtr;
                    Wt::Dbo::ptr<Echoes::Dbo::MessageStatus> mstPtr = m_session.find<Echoes::Dbo::MessageStatus>()
                                    .where(QUOTE(TRIGRAM_MESSAGE_STATUS ID) " = ?").bind(Echoes::Dbo:: EMessageStatus::RECEIVED)
                                    .where(QUOTE(TRIGRAM_MESSAGE_STATUS SEP "DELETE") " IS NULL");
                    newStateMsg->statut = mstPtr;

                    Wt::Dbo::ptr<Echoes::Dbo::MessageTrackingEvent> newMsgTrEv = m_session.add<Echoes::Dbo::MessageTrackingEvent>(newStateMsg);
                }
                else
                {
                    Wt::log("error") << "[Itooki ack Receiver] No Message " << refenvoi;
                    res = EReturnCode::BAD_REQUEST;
                    responseMsg = "no message";   
                }
            }
            else
            {
                Wt::Dbo::ptr<Echoes::Dbo::Message> msgPtr = m_session.find<Echoes::Dbo::Message>()
                        .where(QUOTE(TRIGRAM_MESSAGE SEP "REF" SEP "ACK") " = ?").bind(refenvoi)
                        .where(QUOTE(TRIGRAM_MESSAGE SEP "DELETE") " IS NULL");
                
                if(msgPtr)
                {
                    Echoes::Dbo::MessageTrackingEvent *newStateMsg = new Echoes::Dbo::MessageTrackingEvent();

                    newStateMsg->date = now;
                    newStateMsg->message = msgPtr;
                    Wt::Dbo::ptr<Echoes::Dbo::MessageStatus> mstPtr = m_session.find<Echoes::Dbo::MessageStatus>()
                                    .where(QUOTE(TRIGRAM_MESSAGE_STATUS ID) " = ?").bind(Echoes::Dbo:: EMessageStatus::ACKFAILED)
                                    .where(QUOTE(TRIGRAM_MESSAGE_STATUS SEP "DELETE") " IS NULL");
                    newStateMsg->statut = mstPtr;

                    Wt::Dbo::ptr<Echoes::Dbo::MessageTrackingEvent> newMsgTrEv = m_session.add<Echoes::Dbo::MessageTrackingEvent>(newStateMsg);
                }
                else
                {
                    Wt::log("error") << "[Itooki Ack Receiver] No Message " << refenvoi ;
                    res = EReturnCode::BAD_REQUEST;
                    responseMsg = "no message";   
                }
            }
     }
     else
        {
            res = EReturnCode::BAD_REQUEST;
            const string err = "[Send Resource] sRequest is not empty";
            responseMsg = httpCodeToJSON(res, err);
        }
    return (res);
}

EReturnCode ItookiAckReceiver::processPostRequest(const Wt::Http::Request &request, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, long long> parameters;
    
    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);

    if (nextElement.empty())
    {
        res = postAck(parameters, pathElements, sRequest, responseMsg);
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        responseMsg = "chemin incorrect";
    }

    return res;
}
