#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE test_alert_resource
#include <string>
#include <boost/test/unit_test.hpp>

#include <Wt/Test/WTestEnvironment>
#include <Wt/Json/Array>
#include <Wt/Json/Value>

#define private public
#define protected public
#include "AddonResource.h"

#define unused __attribute__((unused)) static const 

using namespace std;

static const unsigned short CALL_RESULT_OK = Echoes::Dbo::EReturnCode::OK;
static const unsigned short CALL_RESULT_SERVICE_UNAVAILABLE = Echoes::Dbo::EReturnCode::SERVICE_UNAVAILABLE;
// JSON properties for addon resource
static const string ALERT_ID = "id";
static const string ALERT_NAME = "name";
static const string ALERT_DELETE = "delete";


static const string MESSAGE_KEY = "message";
static const string MESSAGE_EXPECTED = "Service Unavailable";

BOOST_AUTO_TEST_SUITE (test_get_alert_resource)

BOOST_AUTO_TEST_CASE(test_get_alerts)
{
    Wt::Test::WTestEnvironment environment("/","wt_config.xml");
    AddonResource addonResource;

    conf.readProperties(*(environment.server()));
    addonResource.m_session.initConnection(conf.getSessConnectParams());

    string responseMsg = "";
    unsigned short result = addonResource.getAddonList(responseMsg);
    
    BOOST_CHECK_EQUAL( result, CALL_RESULT_OK);

    try
    {              
        Wt::Json::Value resValue ;
        Wt::Json::Array addons;
        Wt::Json::parse(responseMsg, resValue);
  
        addons = resValue;
        
        BOOST_CHECK_EQUAL( addons.size(), ADDONS_NUMBER );
        
        for (Wt::Json::Array::const_iterator idx1 = addons.begin() ; idx1 < addons.end(); idx1++)
        {
            Wt::Json::Object tmp = *idx1;
            BOOST_CHECK_EQUAL( tmp.size(), ADDON_JSON_PROPERTIES_NUMBER );
            BOOST_CHECK_EQUAL( tmp.contains(ADDON_ID), true );
            BOOST_CHECK_EQUAL( tmp.contains(ADDON_NAME), true );
            BOOST_CHECK_EQUAL( tmp.contains(ADDON_DELETE), true );
            BOOST_CHECK_EQUAL( tmp.contains(ADDON_SOURCES), true );
            BOOST_CHECK_EQUAL( tmp.contains(ADDON_SEARCH_TYPES), true );
            BOOST_CHECK_EQUAL( tmp.contains(ADDON_SOURCES_PARAMETERS), true );
            unused long long unitId = tmp.get(ADDON_ID);
            unused Wt::WString name = tmp.get(ADDON_NAME);
            unused Wt::WString sDelete = tmp.get(ADDON_DELETE);
            unused int sourcesNumber = tmp.get(ADDON_SOURCES);
            unused int searchTypes = tmp.get(ADDON_SEARCH_TYPES);
            unused int sourcesParameters = tmp.get(ADDON_SOURCES_PARAMETERS);
        }
    }   
    catch (Wt::Json::ParseError const& e)
    {
        BOOST_ERROR("Parse error: " << e.what());
    }
    catch (Wt::Json::TypeException const& e)
    {
        BOOST_ERROR("Type error: " << e.what());
    }   
}

BOOST_AUTO_TEST_CASE(test_get_addons_no_base)
{
    Wt::Test::WTestEnvironment environment("/","src/tests/wt_config_tests.xml");
    AddonResource addonResource;

    conf.readProperties(*(environment.server()));
    addonResource.m_session.initConnection(conf.getSessConnectParams());

    string responseMsg = "";
    unsigned short result = addonResource.getAddonList(responseMsg);
    
    BOOST_CHECK_EQUAL(result, CALL_RESULT_SERVICE_UNAVAILABLE);

    try
    {              
        Wt::Json::Object resObject;
        Wt::Json::parse(responseMsg, resObject);
        
        BOOST_CHECK_EQUAL( resObject.contains(MESSAGE_KEY), true );
        const Wt::WString message = resObject.get(MESSAGE_KEY);
        BOOST_CHECK_EQUAL( message, MESSAGE_EXPECTED );
    }   
    catch (Wt::Json::ParseError const& e)
    {
        BOOST_ERROR("Parse error: " << e.what());
    }
    catch (Wt::Json::TypeException const& e)
    {
        BOOST_ERROR("Type error: " << e.what());
    }   
}

BOOST_AUTO_TEST_SUITE_END()
