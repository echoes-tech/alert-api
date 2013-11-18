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
static const string ADDON_JSON_FILE_NAME =  "tests/AddonResourceTest/addon.json";

BOOST_AUTO_TEST_SUITE (test_get_addon_resource)

BOOST_AUTO_TEST_CASE(test_get_addons)
{
    AddonResource addonResource;
    addonResource.setPathElementsVector("/");
    testGet(addonResource, boost::bind(&AddonResource::getAddonList, &addonResource, _1), ADDONS_JSON_FILE_NAME);
}

BOOST_AUTO_TEST_CASE(test_get_addons_no_base)
{
    AddonResource addonResource;
    addonResource.setPathElementsVector("/");
    testNoBase(addonResource, boost::bind(&AddonResource::getAddonList, &addonResource, _1));
}

BOOST_AUTO_TEST_CASE(test_get_addon)
{
    AddonResource addonResource;
    addonResource.setPathElementsVector("/1");
    testGet(addonResource, boost::bind(&AddonResource::getAddon, &addonResource, _1), ADDON_JSON_FILE_NAME);
}

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
