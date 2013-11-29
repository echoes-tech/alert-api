/* 
 * Tests of API AddonResource
 * @author ECHOES Technologies (TSA)
 * @date 01/11/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE test_addon_resource
#include <string>
#include <boost/test/unit_test.hpp>

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
    testGet(addonResource, boost::bind(&AddonResource::getAddonsList, &addonResource, _1), ADDONS_JSON_FILE_NAME);
}

BOOST_AUTO_TEST_CASE(test_get_addons_no_base)
{
    AddonResource addonResource;
    addonResource.setPathElementsVector("/");
    testNoBase(addonResource, boost::bind(&AddonResource::getAddonsList, &addonResource, _1));
}

BOOST_AUTO_TEST_CASE(test_get_addon)
{
    AddonResource addonResource;
    addonResource.setPathElementsVector("/2");
    testGet(addonResource, boost::bind(&AddonResource::getAddon, &addonResource, _1), ADDON_JSON_FILE_NAME);
}

BOOST_AUTO_TEST_CASE(test_get_addon_no_base)
{
    AddonResource addonResource;
    addonResource.setPathElementsVector("/2");
    testNoBase(addonResource, boost::bind(&AddonResource::getAddon, &addonResource, _1));
}

BOOST_AUTO_TEST_SUITE_END()

