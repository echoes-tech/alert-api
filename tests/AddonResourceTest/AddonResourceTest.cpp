#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE test_addon_resource
#include <string>
#include <boost/test/unit_test.hpp>

#include <Wt/Test/WTestEnvironment>
#include <Wt/Json/Array>
#include <Wt/Json/Value>

#define private public
#define protected public
#include "AddonResource.h"

#include "tests.h"

#define unused __attribute__((unused)) static const 

using namespace std;

static const string ADDONS_JSON_FILE_NAME =  "tests/AddonResourceTest/addons.json";

BOOST_AUTO_TEST_SUITE (test_get_addon_resource)

BOOST_AUTO_TEST_CASE(test_get_addons)
{
    AddonResource addonResource;
    
    test(addonResource, boost::bind(&AddonResource::getAddonList, &addonResource, _1), ADDONS_JSON_FILE_NAME);
}

BOOST_AUTO_TEST_CASE(test_get_addons_no_base)
{
    AddonResource addonResource;

    testNoBase(addonResource, boost::bind(&AddonResource::getAddonList, &addonResource, _1));
}

//BOOST_AUTO_TEST_CASE(test_get_addon)
//{
//    Wt::Test::WTestEnvironment environment("/","wt_config.xml");
//    AddonResource addonResource;
//
//    conf.readProperties(*(environment.server()));
//    addonResource.m_session.initConnection(conf.getSessConnectParams());
//    addonResource.setPathElementsVector("/1")
//
//    string responseMsg = "";
//    unsigned short result = addonResource.getAddon(responseMsg);
//    
//    BOOST_CHECK_EQUAL( result, CALL_RESULT_OK);
//
//    try
//    {              
//        Wt::Json::Object resObject ;
//        Wt::Json::parse(responseMsg, resObject);
//  
//        BOOST_CHECK_EQUAL( tmp.contains(ADDON_ID), true );
//        BOOST_CHECK_EQUAL( tmp.contains(ADDON_NAME), true );
//        BOOST_CHECK_EQUAL( tmp.contains(ADDON_DELETE), true );
//        BOOST_CHECK_EQUAL( tmp.contains(ADDON_SOURCES), true );
//        BOOST_CHECK_EQUAL( tmp.contains(ADDON_SEARCH_TYPES), true );
//        BOOST_CHECK_EQUAL( tmp.contains(ADDON_SOURCES_PARAMETERS), true );
//        
//        
//        for (Wt::Json::Array::const_iterator idx1 = addons.begin() ; idx1 < addons.end(); idx1++)
//        {
//            Wt::Json::Object tmp = *idx1;
//            BOOST_CHECK_EQUAL( tmp.size(), ADDON_JSON_PROPERTIES_NUMBER );
//            BOOST_CHECK_EQUAL( tmp.contains(ADDON_ID), true );
//            BOOST_CHECK_EQUAL( tmp.contains(ADDON_NAME), true );
//            BOOST_CHECK_EQUAL( tmp.contains(ADDON_DELETE), true );
//            BOOST_CHECK_EQUAL( tmp.contains(ADDON_SOURCES), true );
//            BOOST_CHECK_EQUAL( tmp.contains(ADDON_SEARCH_TYPES), true );
//            BOOST_CHECK_EQUAL( tmp.contains(ADDON_SOURCES_PARAMETERS), true );
//            unused long long unitId = tmp.get(ADDON_ID);
//            unused Wt::WString name = tmp.get(ADDON_NAME);
//            unused Wt::WString sDelete = tmp.get(ADDON_DELETE);
//            unused int sourcesNumber = tmp.get(ADDON_SOURCES);
//            unused int searchTypes = tmp.get(ADDON_SEARCH_TYPES);
//            unused int sourcesParameters = tmp.get(ADDON_SOURCES_PARAMETERS);
//        }
//    }   
//    catch (Wt::Json::ParseError const& e)
//    {
//        BOOST_ERROR("Parse error: " << e.what());
//    }
//    catch (Wt::Json::TypeException const& e)
//    {
//        BOOST_ERROR("Type error: " << e.what());
//    }   
//}

BOOST_AUTO_TEST_SUITE_END()

  

//
//
////#define BOOST_TEST_MODULE api_addon
////
////BOOST_AUTO_TEST_SUITE(api_addon)
////
////BOOST_AUTO_TEST_CASE(http_client_test)
////{
////    Wt::Test::WTestEnvironment environment;
////    TestFixture app(environment);
////    AddonResource addonResource;
////    
////    environment.server()->addResource(&addonResource, "/addons");
////
////    Wt::Http::Client *c = new Wt::Http::Client(&app);
////    c->done().connect(boost::bind(&TestFixture::onDone, &app, _1, _2));
////
////    std::string ok = "127.0.0.1/addons";
////
////    if (c->get("http://" + ok))
////    {
////        environment.endRequest();
////        app.waitDone();
////        environment.startRequest();
////    }
////}
////
////BOOST_AUTO_TEST_SUITE_END()
//        
//
//#define BOOST_TEST_MODULE stringtest
//
//BOOST_AUTO_TEST_SUITE(stringtest) // name of the test suite is stringtest
//
//BOOST_AUTO_TEST_CASE(test1)
//{
//  /*
//  mystring s;
//  BOOST_CHECK(s.size() == 0);
//  */
//  BOOST_CHECK(0 == 0);
//}
//
//BOOST_AUTO_TEST_CASE(test2)
//{
//  /*
//  mystring s;
//  s.setbuffer("hello world");
//  BOOST_REQUIRE_EQUAL('h', s[0]); // basic test
//  */
//   BOOST_CHECK(0 == 0);
//}
//
//BOOST_AUTO_TEST_SUITE_END()
