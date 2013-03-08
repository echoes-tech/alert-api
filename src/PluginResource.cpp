/* 
 * API PluginResource
 * @author ECHOES Technologies (GDR)
 * @date 10/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */


#include "PluginResource.h"

PluginResource::PluginResource(){
}

unsigned short PluginResource::getKeyValueForInformation(std::string &responseMsg) const
{
    unsigned short res = 500;
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*session);

        Wt::Dbo::ptr<Information2> ptrInfoKey = session->find<Information2>().where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                                                                             .where("\"SEA_ID\" = ?").bind(this->vPathElements[5])
                                                                             .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                             .where("\"INF_VALUE_NUM\" = ?").bind(this->vPathElements[7])
                                                                             .where("\"INU_ID_INU_ID\" = ?").bind(this->vPathElements[9])
                                                                             .limit(1);

        if (!ptrInfoKey)
        {
            res = 404;
            responseMsg = "{\"message\":\"Information not found\"}";
            return res;
        }

        std::string queryString = 
        "SELECT iva FROM \"T_INFORMATION_VALUE_IVA\" iva WHERE \"IVA_ID\" IN ( SELECT \"IVA_ID\" FROM"
        "("
        "SELECT DISTINCT ON (\"IVA_VALUE\") \"IVA_VALUE\", \"IVA_ID\" FROM"
        "(" "SELECT iva.\"IVA_VALUE\", iva.\"IVA_ID\" FROM \"T_INFORMATION_VALUE_IVA\" iva"
        " WHERE \"SEA_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[5]) + 
        " AND \"SRC_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[3]) + 
        " AND \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[1]) + 
        " AND \"INF_VALUE_NUM\" = " + boost::lexical_cast<std::string>(this->vPathElements[7]) + 
        " AND \"INU_ID_INU_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[9]) + 
        " ORDER BY \"IVA_ID\" DESC LIMIT 50) sr"
        " ) sr_sr"
        ")";

        Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue> > collPtrIva = session->query<Wt::Dbo::ptr<InformationValue> >(queryString);

        if(collPtrIva.size() > 0)
        {
            responseMsg = "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue> >::const_iterator i = collPtrIva.begin(); i != collPtrIva.end(); i++)
            { 
                i->modify()->setId(i->id());
                responseMsg += i->modify()->toJSON();
                 ++idx;
                if(collPtrIva.size()-idx > 0)
                {
                    responseMsg.replace(responseMsg.size()-1, 1, "");
                    responseMsg += ",\n";
                }
            }
            responseMsg = "]\n";
            res = 200;
        }
        else
        {
            res = 404;
            responseMsg = "{\"message\":\"Information value not found\"}";
            return res;
        }
    }
    catch (Wt::Dbo::Exception const &e)
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}

unsigned short PluginResource::getInformationListForPlugin(std::string &responseMsg) const
{
    unsigned short res = 500;
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*this->session);
        std::string queryStr = " SELECT inf FROM \"T_INFORMATION_INF\" inf "
                            "WHERE \"INF_DISPLAY\" = TRUE AND \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[1]) +
                            " AND \"INF_DELETE\" IS NULL";

        Wt::Dbo::Query<Wt::Dbo::ptr<Information2> > queryRes = session->query<Wt::Dbo::ptr<Information2> >(queryStr);

       Wt::Dbo::collection<Wt::Dbo::ptr<Information2> > information = queryRes.resultList();

        if(information.size() > 0 )
        {
            responseMsg = "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Information2> >::const_iterator i = information.begin(); i != information.end(); i++) 
            {
                responseMsg +=  i->modify()->toJSON();
                idx++;
                if(information.size()-idx > 0)
                {
                    responseMsg.replace(responseMsg.size()-1, 1, "");
                    responseMsg += ",\n";
                }
            }
            responseMsg = "]\n";
            res = 200;
            transaction.commit();
        }
        else
        {
            res = 404;
            responseMsg = "{\"message\":\"Information not found\"}";
            return res;
        }
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;  
}

unsigned short PluginResource::getSearchForSourceAndPlugin(std::string& responseMsg) const
{
    unsigned short res = 500;
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*session);
        Wt::Dbo::collection<Wt::Dbo::ptr<Search>> seaCollec = session->find<Search>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                                     .where("\"SRC_ID\" = ?").bind(this->vPathElements[3]);
                                                                                                 
        if (seaCollec.size() > 0)
        {
            responseMsg += "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Search> >::const_iterator i = seaCollec.begin(); i != seaCollec.end(); i++) 
            {
                responseMsg += "\t" + i->modify()->toJSON();
                ++idx;
                if(seaCollec.size()-idx > 0)
                {
                    responseMsg.replace(responseMsg.size()-1, 1, "");
                    responseMsg += ",\n";
                }
            }
            responseMsg += "]\n";               

            res = 200;
        }
        else 
        {
            res = 404;
            responseMsg = "{\"message\":\"Search not found\"}";
            return res;
        }
        transaction.commit();
    }
    catch (Wt::Dbo::Exception const &e)
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}

unsigned short PluginResource::getInformationForSeaSrcAndPlg(std::string& responseMsg) const
{
    unsigned short res = 500;
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*session);
        Wt::Dbo::collection<Wt::Dbo::ptr<Information2>> infCollec = session->find<Information2>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                                                 .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                                                                                                 .where("\"SEA_ID\" = ?").bind(this->vPathElements[5]);
                                                                                                 
        if (infCollec.size() > 0)
        {
            responseMsg += "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Information2> >::const_iterator i = infCollec.begin(); i != infCollec.end(); i++) 
            {
                responseMsg += "\t" + i->modify()->toJSON();
                ++idx;
                if(infCollec.size()-idx > 0)
                {
                    responseMsg.replace(responseMsg.size()-1, 1, "");
                    responseMsg += ",\n";
                }
            }
            responseMsg += "]\n";               

            res = 200;
        }
        else 
        {
            res = 404;
            responseMsg = "{\"message\":\"Information not found\"}";
            return res;
        }
        transaction.commit();
    }
    catch (Wt::Dbo::Exception const &e)
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}

unsigned short PluginResource::getSourceForPlugin(std::string& responseMsg) const
{
    unsigned short res = 500;
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*session);
        Wt::Dbo::collection<Wt::Dbo::ptr<Source>> srcCollec = session->find<Source>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1]);

        if (srcCollec.size() > 0)
        {
            responseMsg += "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Source> >::const_iterator i = srcCollec.begin(); i != srcCollec.end(); i++) 
            {
                responseMsg += "\t" + i->modify()->toJSON();
                ++idx;
                if(srcCollec.size()-idx > 0)
                {
                    responseMsg.replace(responseMsg.size()-1, 1, "");
                    responseMsg += ",\n";
                }
            }
            responseMsg += "]\n";               

            res = 200;
        }
        else 
        {
            res = 404;
            responseMsg = "{\"message\":\"Plugin not found\"}";
            return res;
        }
        transaction.commit();
    }
    catch (Wt::Dbo::Exception const &e)
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}

void PluginResource::processGetRequest(Wt::Http::Response &response)
{
    std::string responseMsg = "", nextElement = "";
    
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->statusCode = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned int>(nextElement);

            nextElement = getNextElementFromPath();

            if(!nextElement.compare("informations"))
            {
                nextElement = getNextElementFromPath();
                if(!nextElement.compare("informations"))
                {
                    this->statusCode = getInformationListForPlugin(responseMsg);
                }
                else
                {
                    this->statusCode = 400;
                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                }
            }
            else if (!nextElement.compare("sources"))
            {
                nextElement = getNextElementFromPath();
                if(!nextElement.compare(""))
                {
                    this->statusCode = getSourceForPlugin(responseMsg);
                }
                else
                {
                    boost::lexical_cast<unsigned int>(nextElement);
                    nextElement = getNextElementFromPath();
                    if (!nextElement.compare("searches"))
                    {
                        nextElement = getNextElementFromPath();
                        if(!nextElement.compare(""))
                        {
                            this->statusCode = getSearchForSourceAndPlugin(responseMsg);
                        }
                        else
                        {
                            boost::lexical_cast<unsigned int>(nextElement);
                            nextElement = getNextElementFromPath();
                            if(!nextElement.compare("informations"))
                            {
                                nextElement = getNextElementFromPath();
                                if(!nextElement.compare(""))
                                {
                                    this->statusCode = getInformationForSeaSrcAndPlg(responseMsg);
                                }
                                else
                                {
                                    this->statusCode = 400;
                                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                                }
                                
                            }
                            else if (!nextElement.compare("inf_values"))
                            {
                                nextElement = getNextElementFromPath();
                                boost::lexical_cast<unsigned int>(nextElement);
                                nextElement = getNextElementFromPath();
                                if (!nextElement.compare("units"))
                                {
                                    nextElement = getNextElementFromPath();
                                    boost::lexical_cast<unsigned int>(nextElement);
                                    nextElement = getNextElementFromPath();
                                    if (!nextElement.compare("informations"))
                                    {
                                        this->statusCode = getKeyValueForInformation(responseMsg);
                                    }
                                    else
                                    {
                                        this->statusCode = 400;
                                        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                                    }
                                }
                                else
                                {
                                    this->statusCode = 400;
                                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                                }
                            }
                            else
                            {
                                this->statusCode = 400;
                                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                            }
                        }
                    }
                    else
                    {
                        this->statusCode = 400;
                        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                    }
                }
            }
            else
            {
                this->statusCode = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            this->statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(this->statusCode);
    response.out() << responseMsg;
    return;
}


unsigned short PluginResource::postPlugin(std::string& responseMsg, const std::string& sRequest)
{
    unsigned short res = 500;
    Wt::WString plgName = "", plgDesc = "";
    
    try
    {
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);
        //descriptif
        plgName = result.get("plg_name");
        plgDesc = result.get("plg_desc");
    }
    catch (Wt::Json::ParseError const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON:" << sRequest;
        return res;
    }
    catch (Wt::Json::TypeException const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON.:" << sRequest;
        return res;
    }   
    try 
    {
        Wt::Dbo::Transaction transaction(*session);
        Plugin *plugin = new Plugin();
        plugin->name = plgName;
        plugin->desc = plgDesc;
        
        Wt::Dbo::ptr<Plugin> plgPtr = session->add<Plugin>(plugin);
       
        plgPtr.flush();
        plgPtr.modify()->setId(plgPtr.id());
        responseMsg = plgPtr.modify()->toJSON();
        
        transaction.commit();
        res = 200;
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}

unsigned short PluginResource::postSourceForPlugin(std::string& responseMsg, const std::string& sRequest)
{
    unsigned short res = 500;
    Wt::WString addonId = ""; // plgId = "";
    long long srcId;
    
    try
    {
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);
        int plgIdInt, addonIdInt;
        //descriptif

        addonIdInt = result.get("addon_id");
        addonId = boost::lexical_cast<std::string>(addonIdInt);
//        plgIdInt = result.get("plg_id");
//        plgId = boost::lexical_cast<std::string>(plgIdInt);
        srcId = result.get("src_id");
        
        try
        {
            Wt::Dbo::Transaction transaction(*session);
            //creation de la source
            Wt::Dbo::ptr<Plugin> plgPtr = session->find<Plugin>().where("\"PLG_ID\" = ?").bind(this->vPathElements[1]);
            Wt::Dbo::ptr<Addon> addonPtr = session->find<Addon>().where("\"ADO_ID\" = ?").bind(addonId);
            Source *source = new Source;
            source->pk.id = srcId;
            source->pk.plugin = plgPtr;
            source->addon = addonPtr;
            Wt::Dbo::ptr<Source> srcPtr = session->add<Source>(source);
            srcPtr.flush();


            Wt::Dbo::collection<Wt::Dbo::ptr<SourceParameter> > sourceParameterPtr = addonPtr.get()->sourceParameters;

            //creations des parametres
            for (Wt::Dbo::collection<Wt::Dbo::ptr<SourceParameter> >::const_iterator i = sourceParameterPtr.begin(); i != sourceParameterPtr.end(); i++)
            {
                SourceParameterValue *sourceParameterValue = new SourceParameterValue;
                
                //recuperation des "source_parameters" dans le JSON
                Wt::WString tmp = result.get(boost::lexical_cast<std::string>((*i).get()->name));
                
                sourceParameterValue->value = tmp;
                sourceParameterValue->name = (*i).get()->name;
                sourceParameterValue->pk.sourceParameter = *i;
                sourceParameterValue->pk.source = srcPtr;
                session->add<SourceParameterValue>(sourceParameterValue);
            }
            res = 200;
            responseMsg = srcPtr.modify()->toJSON();
            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e) 
        {
            Wt::log("error") << e.what();
            res = 503;
            responseMsg = "{\"message\":\"Service Unavailable\"}";
            return res;
        }
    }
    catch (Wt::Json::ParseError const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON:" << sRequest;
        return res;
    }
    catch (Wt::Json::TypeException const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON.:" << sRequest;
        return res;
    }   

    return res;
}

unsigned short PluginResource::postSearchForSourceAndPlugin(std::string& responseMsg, const std::string& sRequest)
{
    unsigned short res = 500;
    Wt::WString seaPeriod;// unitId;
    long long seaId, styId;
    bool seaIsStatic;
    int posKeyValue, nbValue;
        
    try
    {
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);
       // int unitIdInt;
        
        //information
//        unitIdInt = result.get("unit_id");
//        unitId = boost::lexical_cast<std::string>(unitIdInt);

        //search
        seaPeriod = result.get("sea_period");
        styId = result.get("sty_id");
        seaId = result.get("sea_id");
        seaIsStatic = result.get("sea_is_static");
        posKeyValue = result.get("pos_key_value");
        nbValue = result.get("nb_value");
        
        try
        {
            Wt::Dbo::Transaction transaction(*session);
            
            //creation de la search
            Wt::Dbo::ptr<Source> srcPtr = session->find<Source>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                 .where("\"SRC_ID\" = ?").bind(this->vPathElements[3]);
            if(srcPtr)
            {
                Wt::Dbo::ptr<SearchType> seaTypPtr = session->find<SearchType>().where("\"STY_ID\" = ?").bind(styId);
                if(seaTypPtr)
                {
                    Search *search = new Search;
                    search->pk.id = seaId;
                    search->pk.source = srcPtr;
                    search->searchType = seaTypPtr;
                    search->period = seaPeriod;
                    search->nbValue = nbValue;
                    search->pos_key_value = posKeyValue;
                    search->searchIsStatic = seaIsStatic;
                    Wt::Dbo::ptr<Search> seaPtr = session->add<Search>(search);
                    seaPtr.flush();


                    Wt::Dbo::collection<Wt::Dbo::ptr<SearchParameter> > searchParameterPtr = seaTypPtr.get()->searchParameters;
                    //creations des parametres
                    for (Wt::Dbo::collection<Wt::Dbo::ptr<SearchParameter> >::const_iterator i = searchParameterPtr.begin(); i != searchParameterPtr.end(); i++)
                    {
                        SearchParameterValue *searchParameterValue = new SearchParameterValue;

                        //recuperation des "search_parameters" dans le JSON
                        Wt::WString tmp = result.get(boost::lexical_cast<std::string>((*i).get()->name));

                        searchParameterValue->value = tmp;
                        searchParameterValue->name = (*i).get()->name;
                        searchParameterValue->searchParameterValueId.searchParameter = *i;
                        searchParameterValue->searchParameterValueId.search = seaPtr;
                        session->add<SearchParameterValue>(searchParameterValue);
                    }

//                    Wt::Dbo::ptr<InformationUnit> informationUnitPtr = session->find<InformationUnit>().where("\"INU_ID\" = ?").bind(unitId);
//
//                    SearchUnit *searchUnit = new SearchUnit;
//                    searchUnit->pk.infValueNum = posKeyValue;
//                    searchUnit->pk.search = seaPtr;
//                    searchUnit->informationUnit = informationUnitPtr;
//                    session->add<SearchUnit>(searchUnit);
                }
                else
                {
                    res = 404;
                    responseMsg = "{\"message\":\"Search Type not found\"}";
                    return res;
                }
            }
            else
            {
                res = 404;
                responseMsg = "{\"message\":\"Source not found\"}";
                return res;
            }

            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e) 
        {
            Wt::log("error") << e.what();
            res = 503;
            responseMsg = "{\"message\":\"Service Unavailable\"}";
            return res;
        }
    }
    catch (Wt::Json::ParseError const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON:" << sRequest;
        return res;
    }
    catch (Wt::Json::TypeException const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON.:" << sRequest;
        return res;
    }   
    return res;
}

unsigned short PluginResource::postInformationForSeaSrcAndPlg(std::string& responseMsg, const std::string& sRequest)
{
    unsigned short res = 500;
    Wt::WString infName, unitId, infCalculate;
    bool infDisplay;
    int valueNum;
         
    try
    {
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);
        int unitIdInt;
        
        //information
        unitIdInt = result.get("unit_id");
        unitId = boost::lexical_cast<std::string>(unitIdInt);
        infName = result.get("inf_name");
        infDisplay = result.get("inf_display");
        infCalculate = result.get("inf_calculate");
        valueNum = result.get("inf_value_num");
        
        try
        {
            Wt::Dbo::Transaction transaction(*session);
            
            //search exist ?
            Wt::Dbo::ptr<Search> seaPtr = session->find<Search>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                 .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                                                                 .where("\"SEA_ID\" = ?").bind(this->vPathElements[5]);
            
            if(seaPtr)
            {
                //Relier une unité à l'info
                // unit exist?
                Wt::Dbo::ptr<InformationUnit> informationUnitPtr = session->find<InformationUnit>().where("\"INU_ID\" = ?").bind(unitId);
                if(informationUnitPtr)
                {
                    SearchUnit *searchUnit = new SearchUnit;
                    searchUnit->pk.infValueNum = valueNum;
                    searchUnit->pk.search = seaPtr;
                    searchUnit->informationUnit = informationUnitPtr;
                    session->add<SearchUnit>(searchUnit);

                    //creation info
                    Information2 *information = new Information2;
                    information->pk.search = seaPtr;
                    information->pk.unit = informationUnitPtr;
                    information->pk.subSearchNumber = valueNum;
                    information->name = infName;
                    information->display = infDisplay;
                    information->calculate = infCalculate;
                    Wt::Dbo::ptr<Information2> infPtr = session->add<Information2>(information);
                    infPtr.flush();
                    responseMsg = infPtr.modify()->toJSON();
                }
                else
                {
                    res = 404;
                    responseMsg = "{\"message\":\"Information not found\"}";
                    return res;
                }
            }
            else
            {
                res = 404;
                responseMsg = "{\"message\":\"Search not found\"}";
                return res;
            }
            res = 200;
            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e) 
        {
            Wt::log("error") << e.what();
            res = 503;
            responseMsg = "{\"message\":\"Service Unavailable\"}";
            return res;
        }
    }
    catch (Wt::Json::ParseError const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON:" << sRequest;
        return res;
    }
    catch (Wt::Json::TypeException const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON.:" << sRequest;
        return res;
    }   
    return res; 
}

void PluginResource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    
 std::string responseMsg = "", nextElement = "", sRequest = "";

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->statusCode = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}"; 
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned int>(nextElement);
            nextElement = getNextElementFromPath();
            if(!nextElement.compare(""))
            {
                this->statusCode = deletePlugin(responseMsg);
            }
            else if(!nextElement.compare("sources"))
            {
                nextElement = getNextElementFromPath();
                boost::lexical_cast<unsigned int>(nextElement);

                nextElement = getNextElementFromPath();
                if(!nextElement.compare(""))
                {
                    this->statusCode = deleteSourceForPlugin(responseMsg);
                }
                else if(!nextElement.compare("searches"))
                {
                    
                    nextElement = getNextElementFromPath();
                    boost::lexical_cast<unsigned int>(nextElement);

                    nextElement = getNextElementFromPath();

                    if(!nextElement.compare(""))
                    {
                        this->statusCode = deleteSearchForSourceAndPlugin(responseMsg);
                    }
                    else if (!nextElement.compare("inf_values"))
                    {
                        nextElement = getNextElementFromPath();
                        boost::lexical_cast<unsigned int>(nextElement);
                        nextElement = getNextElementFromPath();
                        if (!nextElement.compare("units"))
                        {
                            nextElement = getNextElementFromPath();
                            boost::lexical_cast<unsigned int>(nextElement);
                            nextElement = getNextElementFromPath();
                            if (!nextElement.compare("informations"))
                            {
                                this->statusCode = deleteInformationForSeaSrcAndPlg(responseMsg);
                            }
                            else
                            {
                                this->statusCode = 400;
                                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                            }
                        }
                        else
                        {
                            this->statusCode = 400;
                            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                        }
                    }
                    else
                    {
                        this->statusCode = 400;
                        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                    }
                }
                else
                {
                    this->statusCode = 400;
                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                }
            }
            else
            {
                this->statusCode = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            this->statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(this->statusCode);
    response.out() << responseMsg;
    return;


//    std::string responseMsg = "", nextElement = "", sRequest = "";
//
//    sRequest = request2string(request);
//    nextElement = getNextElementFromPath();
//    if(!nextElement.compare(""))
//    {
//        this->statusCode = postPlugin(responseMsg, sRequest);
//    }
//    else
//    {       
//        try
//        {
//            boost::lexical_cast<unsigned int>(nextElement);
//
//            nextElement = getNextElementFromPath();
//
//            if(!nextElement.compare("sources"))
//            {
//                nextElement = getNextElementFromPath();
//                if (!nextElement.compare(""))
//                {
//                    this->statusCode = postSourceForPlugin(responseMsg, sRequest);
//                }
//                else
//                {
//                    boost::lexical_cast<unsigned int>(nextElement);
//
//                    nextElement = getNextElementFromPath();
//
//                    if(!nextElement.compare("searches"))
//                    {
//                        nextElement = getNextElementFromPath();
//                        if (!nextElement.compare(""))
//                        {
//                            this->statusCode = postSearchForSourceAndPlugin(responseMsg, sRequest);
//                        }
//                        else
//                        {
//                            boost::lexical_cast<unsigned int>(nextElement);
//                            nextElement = getNextElementFromPath();
//
//                            if(!nextElement.compare("informations"))
//                            {
//                                nextElement = getNextElementFromPath();
//                                if (!nextElement.compare(""))
//                                {
//                                    this->statusCode = postInformationForSeaSrcAndPlg(responseMsg, sRequest);
//                                }
//                                else
//                                {
//                                    this->statusCode = 400;
//                                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                                }
//                            }
//                            else
//                            {
//                                this->statusCode = 400;
//                                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                            }
//                        }
//                    }
//                    else
//                    {
//                        this->statusCode = 400;
//                        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                    }
//                }
//            }
//            else
//            {  
//                this->statusCode = 400;
//                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//            }
//        }
//        catch(boost::bad_lexical_cast &)
//        {
//            this->statusCode = 400;
//            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//        }             
//    }
//
//    response.setStatus(this->statusCode);
//    response.out() << responseMsg;
//    return ;
}


void PluginResource::processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}


void PluginResource::processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}

unsigned short PluginResource::deleteInformationForSeaSrcAndPlg(std::string& responseMsg)
{
    
    unsigned short res = 500;
    try 
    {  
        Wt::Dbo::Transaction transaction(*this->session);
           
        Wt::Dbo::ptr<Information2> informationPtr = session->find<Information2>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                             .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                                                             .where("\"SEA_ID\" = ?").bind(this->vPathElements[5])
                                                             .where("\"INF_VALUE_NUM\" = ?").bind(this->vPathElements[7])
                                                             .where("\"INU_ID_INU_ID\" = ?").bind(this->vPathElements[9]);
        //information exist ?
        if(informationPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<AlertValue>> avaCollec = session->find<AlertValue>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                                                 .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                                                                                                 .where("\"SEA_ID\" = ?").bind(this->vPathElements[5])
                                                                                                 .where("\"INF_VALUE_NUM\" = ?").bind(this->vPathElements[7])
                                                                                                 .where("\"INU_ID_INU_ID\" = ?").bind(this->vPathElements[9]);
            //verif si l'info n'est pas utilisée                                                                
            if (avaCollec.size() == 0)
            {                
                //supprime l'info

                std::string executeString1 = "DELETE FROM \"T_INFORMATION_INF\" "
                                             "WHERE \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[1]) +
                                                " AND \"SRC_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[3]) +
                                                " AND \"SEA_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[5]) +
                                                " AND \"INF_VALUE_NUM\" = " + boost::lexical_cast<std::string>(this->vPathElements[7]) +
                                                " AND \"INU_ID_INU_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[9]);
                
                //supprime search unit
                std::string executeString2 = "DELETE FROM \"T_SEARCH_UNIT_SEU\" "
                                             "WHERE \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[1]) +
                                                " AND \"SRC_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[3]) +
                                                " AND \"SEA_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[5]) +
                                                " AND \"INF_VALUE_NUM\" = " + boost::lexical_cast<std::string>(this->vPathElements[7]);
                                                                
                session->execute(executeString1);
                session->execute(executeString2);
            }
            else
            {
                res = 409;
                responseMsg = "{\"message\":\"Conflict, an information use this search\"}";
                return res;
            }
            
        }
        else
        {
            responseMsg = "{\"message\":\"Information Not Found\"}";
            res = 404;
            return res;
        }
        res = 204;
        transaction.commit();               
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    
    return res;
}

unsigned short PluginResource::deleteSearchForSourceAndPlugin(std::string& responseMsg)
{
    unsigned short res = 500;
    try 
    {  
        Wt::Dbo::Transaction transaction(*this->session);
           
        Wt::Dbo::ptr<Search> seaPtr = session->find<Search>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                             .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                                                             .where("\"SEA_ID\" = ?").bind(this->vPathElements[5]);
        //search exist ?
        if(seaPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<Information2>> infCollec = session->find<Information2>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                                                 .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                                                                                                 .where("\"SEA_ID\" = ?").bind(this->vPathElements[5]);
            //verif si la search n'est pas utilisé                                                                
            if (infCollec.size() == 0)
            {                
                //supprime les parm liés a la search
                std::string executeString1 = " DELETE FROM \"T_SEARCH_PARAMETER_VALUE_SEV\" " 
                                                " WHERE \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[1]) +
                                                " AND \"SRC_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[3]) +
                                                " AND \"SEA_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[5]) +
                                                " AND \"SEP_ID_SEP_ID\" IN "
                                                "("
                                                    "SELECT \"T_SEARCH_PARAMETER_SEP_SEP_ID\" FROM \"TJ_STY_SEP\" "
                                                    "WHERE \"T_SEARCH_TYPE_STY_STY_ID\" = " + boost::lexical_cast<std::string>(seaPtr.get()->searchType.id()) +
                                                ")";
                //std::cerr << executeString1;
                std::string executeString2 = "DELETE FROM \"T_SEARCH_SEA\" "
                                             "WHERE \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[1]) +
                                                " AND \"SRC_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[3]) +
                                                " AND \"SEA_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[5]);
                session->execute(executeString1);
                session->execute(executeString2);
            }
            else
            {
                res = 409;
                responseMsg = "{\"message\":\"Conflict, an information use this search\"}";
                return res;
            }
            
        }
        else
        {
            responseMsg = "{\"message\":\"Search Not Found\"}";
            res = 404;
            return res;
        }
        res = 204;
        transaction.commit();               
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    
    return res;
    
}

unsigned short PluginResource::deletePlugin(std::string& responseMsg)
{
    unsigned short res = 500;
    try 
    {  
        Wt::Dbo::Transaction transaction(*this->session);
        Wt::Dbo::ptr<Plugin> plgPtr = session->find<Plugin>().where("\"PLG_ID\" = ?").bind(this->vPathElements[1]);
        //verif si le plugin existe
        if(plgPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<Source>> srcCollec = session->find<Source>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1]);
            //verif si le plugin n'est pas utilisé
            if(srcCollec.size() == 0)
            {
                std::string queryString = "DELETE FROM \"T_PLUGIN_PLG\" "
                                 "WHERE \"PLG_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[1]);
                session->execute(queryString);
            }
            else
            {
                res = 409;
                responseMsg = "{\"message\":\"Conflict, a source use this plugin\"}";
                return res;
            }
        }
        else
        {
            res = 404;
            responseMsg = "{\"message\":\"Plugin not found\"}";
            return res;
        }
        res = 204;
        transaction.commit();               
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    
    return res;
}

unsigned short PluginResource::deleteSourceForPlugin(std::string& responseMsg)
{
    unsigned short res = 500;
    try 
    {  
        Wt::Dbo::Transaction transaction(*this->session);
        Wt::Dbo::ptr<Source> srcPtr = session->find<Source>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                             .where("\"SRC_ID\" = ?").bind(this->vPathElements[3]);
        //verif si la source existe
        if(srcPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<Search>> seaCollec = session->find<Search>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                 .where("\"SRC_ID\" = ?").bind(this->vPathElements[3]);
            //verif si la source n'est pas utilisée
            if(seaCollec.size() == 0)
            {
                std::string queryString = "DELETE FROM \"T_SOURCE_PARAMETER_VALUE_SPV\" "
                                 "WHERE \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[1]) +
                                 "AND \"SRC_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[3]) +
                                 "AND \"SRP_ID_SRP_ID\" IN "
                                 "("
                                        "SELECT \"T_SOURCE_PARAMETER_SRP_SRP_ID\" FROM \"TJ_ADO_SRP\" "
                                        "WHERE \"T_ADDON_ADO_ADO_ID\" = " + boost::lexical_cast<std::string>(srcPtr.get()->addon.id()) +
                                 ")";
                session->execute(queryString);
                std::string queryString2 = "DELETE FROM \"T_SOURCE_SRC\" "
                                           "WHERE \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[1]) +
                                           "AND \"SRC_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[3]);
                session->execute(queryString2);
            }
            else
            {
                res = 409;
                responseMsg = "{\"message\":\"Conflict, a search use this source\"}";
                return res;
            }
        }
        else
        {
            res = 404;
            responseMsg = "{\"message\":\"Source not found\"}";
            return res;
        }
        res = 204;
        transaction.commit();               
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    
    return res;
    
}

void PluginResource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    std::string responseMsg = "", nextElement = "", sRequest = "";

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->statusCode = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}"; 
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned int>(nextElement);
            nextElement = getNextElementFromPath();
            if(!nextElement.compare(""))
            {
                this->statusCode = deletePlugin(responseMsg);
            }
            else if(!nextElement.compare("sources"))
            {
                nextElement = getNextElementFromPath();
                boost::lexical_cast<unsigned int>(nextElement);

                nextElement = getNextElementFromPath();
                if(!nextElement.compare(""))
                {
                    this->statusCode = deleteSourceForPlugin(responseMsg);
                }
                else if(!nextElement.compare("searches"))
                {
                    
                    nextElement = getNextElementFromPath();
                    boost::lexical_cast<unsigned int>(nextElement);

                    nextElement = getNextElementFromPath();

                    if(!nextElement.compare(""))
                    {
                        this->statusCode = deleteSearchForSourceAndPlugin(responseMsg);
                    }
                    else if (!nextElement.compare("inf_values"))
                    {
                        nextElement = getNextElementFromPath();
                        boost::lexical_cast<unsigned int>(nextElement);
                        nextElement = getNextElementFromPath();
                        if (!nextElement.compare("units"))
                        {
                            nextElement = getNextElementFromPath();
                            boost::lexical_cast<unsigned int>(nextElement);
                            nextElement = getNextElementFromPath();
                            if (!nextElement.compare("informations"))
                            {
                                this->statusCode = deleteInformationForSeaSrcAndPlg(responseMsg);
                            }
                            else
                            {
                                this->statusCode = 400;
                                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                            }
                        }
                        else
                        {
                            this->statusCode = 400;
                            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                        }
                    }
                    else
                    {
                        this->statusCode = 400;
                        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                    }
                }
                else
                {
                    this->statusCode = 400;
                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                }
            }
            else
            {
                this->statusCode = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            this->statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(this->statusCode);
    response.out() << responseMsg;
    return;
}


void PluginResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    return;
}


PluginResource::~PluginResource()
{
    beingDeleted();
}
