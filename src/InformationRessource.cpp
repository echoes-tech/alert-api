/* 
 * API InformationRessource
 * @author ECHOES Technologies (GDR)
 * @date 11/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */


#include "InformationRessource.h"


using namespace std;

InformationRessource::InformationRessource(){
}


void InformationRessource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
     // Create Session and Check auth
   // PublicApiResource::handleRequest(request, response);
    session = new Session(Utils::connection);
    // set Content-Type
    response.setMimeType("application/json; charset=utf-8");
    
  /*  if (!this->authentified) {
        response.setStatus(401);
        response.out() << "{\"message\":\"Authentification Failed\"}";
        return;
    }*/
  
    
  // URL path after /information
    string path = request.pathInfo();

    vector< string > splitPath;
    boost::split(splitPath, path, boost::is_any_of("/"), boost::token_compress_on);
    
     if (request.method() == "GET") 
     {
         switch (splitPath.size()) 
         {
             case 2:
                if (splitPath[1] == "criterias") //renvoie la liste des critére
                {
                    try
                    {
                        Wt::Dbo::Transaction transaction(*session);
                        Wt::Dbo::collection<Wt::Dbo::ptr<AlertCriteria> > alertCriterias = session->find<AlertCriteria>();
                        for (Wt::Dbo::collection<Wt::Dbo::ptr<AlertCriteria> >::const_iterator i = alertCriterias.begin(); i != alertCriterias.end(); ++i)
                        {
                            response.out() << "{\n\""
                                            << "  \"acr_id\" : \"" << i->id() << "\"\n\""
                                            << "  \"acr_name\" : \"" << i->get()->name << "\"\n\""
                                        << "}\n";
                        }
                        response.setStatus(200);
                        transaction.commit();
                    }
                    catch (Wt::Dbo::Exception e)
                    {
                        Wt::log("error") << e.what();
                        response.setStatus(503);
                        response.out() << "{\"message\":\"Service Unavailable\"}";
                        return;
                    }
                }
                else
                {
                    response.setStatus(422);
                    response.out() << "{\"message\":\"Validation Failed\"}";
                }
                break;
             case 3:
                if (splitPath[1] == "unit") //renvoie l'unité correspondant à une info
                {               
                    try
                    {
                        this->unitId = boost::lexical_cast<int>(splitPath[2]);
                    }
                    catch(boost::bad_lexical_cast &)
                    {
                        response.setStatus(422);
                        response.out() << "{\"message\":\"Validation Failed\"}";
                        return;
                    }
                    try
                    {
                        Wt::Dbo::Transaction transaction(*this->session);
                        
                        Wt::Dbo::collection<Wt::Dbo::ptr<InformationSubUnit> > infoSubUnit = session->find<InformationSubUnit>()
                                                                                             .where("\"ISU_INU_INU_ID\" = ?").bind(this->unitId);
                       
                        Wt::Dbo::ptr<InformationUnit> informationUnit = session->find<InformationUnit>()
                                                                          .where("\"INU_ID\" = ?").bind(this->unitId);
                        if (!informationUnit)
                        {
                            response.setStatus(404);
                            response.out() << "{\"message\":\"Unit not found\"}";
                            return;
                        }
                        else
                        {
                            response.out() << "{\n\""
                                            << "  \"inu_name\" : \"" << informationUnit.get()->name.toUTF8() << "\"\n\""
                                        << "}\n";
                            for (Wt::Dbo::collection<Wt::Dbo::ptr<InformationSubUnit> >::const_iterator i = infoSubUnit.begin(); i != infoSubUnit.end(); i++)
                            {
                                response.out() << "{\n\""
                                            << "  \"isu_name\" : \"" << i->get()->name.toUTF8() << "\"\n\""
                                        << "}\n";
                            }  
                            response.setStatus(200);
                        }
                        
                        transaction.commit();
                    }
                    catch (Wt::Dbo::Exception const &e)
                    {
                        Wt::log("error") << e.what();
                        response.setStatus(503);
                        response.out() << "{\"message\":\"Service Unavailable\"}";
                        return;
                    }
                }
                else
                {
                    response.setStatus(422);
                    response.out() << "{\"message\":\"Validation Failed\"}";
                }
                break;
                
             case 7:
                if(splitPath[1] == "key_value") //
                {
                    int seaId, srcId, plgId, valNum, astId;
                    try 
                    {
                        seaId = boost::lexical_cast<int>(splitPath[2]);
                        srcId = boost::lexical_cast<int>(splitPath[3]);
                        plgId = boost::lexical_cast<int>(splitPath[4]);
                        valNum = boost::lexical_cast<int>(splitPath[5]);
                        astId = boost::lexical_cast<int>(splitPath[6]);
                    }    
                    catch (boost::bad_lexical_cast &) 
                    {
                        response.setStatus(422);
                        response.out() << "{\"message\":\"Validation Failed\"}";
                        return;
                    }
                    try
                    {
                        /////
                    
                        Wt::Dbo::Transaction transaction(*session);
  
                        Wt::Dbo::ptr<Information2> ptrInfoKey = session->find<Information2>().where("\"SEA_ID\" = ?").bind(seaId)
                                                    .where("\"SRC_ID\" = ?").bind(srcId)
                                                    .where("\"PLG_ID_PLG_ID\" = ?").bind(plgId)
                                                    .where("\"INF_VALUE_NUM\" = ?").bind(valNum)
                                                    .limit(1);

                        if (!ptrInfoKey)
                        {
                            response.setStatus(404);
                            response.out() << "{\"message\":\"Information not found\"}";
                            return;
                        }

                        std::string queryString = 
                        "SELECT iva FROM \"T_INFORMATION_VALUE_IVA\" iva WHERE \"IVA_ID\" IN ( SELECT \"IVA_ID\" FROM"
                        "("
                        "SELECT DISTINCT ON (\"IVA_VALUE\") \"IVA_VALUE\", \"IVA_ID\" FROM"
                        "(" "SELECT iva.\"IVA_VALUE\", iva.\"IVA_ID\" FROM \"T_INFORMATION_VALUE_IVA\" iva"
                        " WHERE \"SEA_ID\" = " + boost::lexical_cast<std::string>(seaId) + 
                        " AND \"SRC_ID\" = " + boost::lexical_cast<std::string>(srcId) + 
                        " AND \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<std::string>(plgId) + 
                        " AND \"INF_VALUE_NUM\" = " + boost::lexical_cast<std::string>(valNum) + 
                        " AND \"IVA_AST_AST_ID\" = " + boost::lexical_cast<std::string>(astId) + " ORDER BY \"IVA_ID\" DESC LIMIT 50) sr"
                        " ) sr_sr"
                        ")";

                        Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue> > collPtrIva = session->query<Wt::Dbo::ptr<InformationValue> >(queryString);
                       
                        if(collPtrIva.size() > 0)
                        {
                            for (Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue> >::const_iterator i = collPtrIva.begin(); i != collPtrIva.end(); i++)
                            {
                                response.out() << "{\n\""
                                                << "  \"iva_value\" : \"" << i->id() << "\"\n\""
                                            << "}\n";
                            }
                            response.setStatus(200);
                        }
                        else
                        {
                            response.setStatus(404);
                            response.out() << "{\"message\":\"Information value not found\"}";
                            return;
                        }

                        /////
                    }
                    catch (Wt::Dbo::Exception const &e)
                    {
                        Wt::log("error") << e.what();
                        response.setStatus(503);
                        response.out() << "{\"message\":\"Service Unavailable\"}";
                        return;
                    }
                }
                else
                {
                    response.setStatus(422);
                    response.out() << "{\"message\":\"Validation Failed\"}";
                }
                break;
                
            default :
                response.setStatus(422);
                response.out() << "{\"message\":\"Validation Failed\"}";
                break;
         }
     }
     else 
     { 
        response.setStatus(405);
        response.out() << "{\"message\":\"Only GET method is allowed.\"}";
        return;
        
     }
}


InformationRessource::~InformationRessource()
{
    beingDeleted();
}