/* 
 * Header of Tests
 * @author ECHOES Technologies (FPO-TSA)
 * @date 17/11/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef TEST_H
#define	TEST_H

#include <Wt/Test/WTestEnvironment>
#include <Wt/Json/Array>
#include <Wt/Json/Value>

static const EReturnCode CALL_RESULT_OK = EReturnCode::OK;
static const EReturnCode CALL_RESULT_SERVICE_UNAVAILABLE = EReturnCode::SERVICE_UNAVAILABLE;

static const std::string MESSAGE_KEY = "message";
static const std::string MESSAGE_EXPECTED = "Service Unavailable";

template<class C>
void testGet(C &obj, boost::function<EReturnCode (std::string&)> func, const std::string& filename)
{
    Wt::Test::WTestEnvironment environment("/", "wt_config.xml");

    conf.readProperties(*(environment.server()));
    obj.m_session.initConnection(conf.getSessConnectParams());

    std::string responseMsg = "";
    EReturnCode result = func(responseMsg);

    std::ifstream file(filename);
    if (file.is_open())
    {
        std::string tmpResString((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        BOOST_CHECK_EQUAL(responseMsg, tmpResString);
        file.close();
    }
    else
    {
        BOOST_ERROR(filename + " not found");
    }

    BOOST_CHECK_EQUAL(result, CALL_RESULT_OK);
}

template<class C>
void testNoBase(C &obj, boost::function<EReturnCode (std::string&)> func )
{
    Wt::Test::WTestEnvironment environment("/", "tests/wt_config_tests.xml");

    conf.readProperties(*(environment.server()));
    obj.m_session.initConnection(conf.getSessConnectParams());

    std::string responseMsg = "";
    EReturnCode result = func(responseMsg);

    BOOST_CHECK_EQUAL(result, CALL_RESULT_SERVICE_UNAVAILABLE);

    try
    {
        Wt::Json::Object resObject;
        Wt::Json::parse(responseMsg, resObject);

        BOOST_CHECK_EQUAL(resObject.contains(MESSAGE_KEY), true);
        const Wt::WString message = resObject.get(MESSAGE_KEY);
        BOOST_CHECK_EQUAL(message, MESSAGE_EXPECTED);
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

#endif /* TEST_H */

