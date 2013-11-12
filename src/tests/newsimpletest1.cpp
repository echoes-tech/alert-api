#define BOOST_TEST_DYN_LINK

#define BOOST_TEST_MODULE stringtest
#include <boost/test/unit_test.hpp>

#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>

#include <Wt/Test/WTestEnvironment>
#include <Wt/WApplication>
#include <Wt/WIOService>
#include <Wt/Http/Client>
#include <Wt/WServer>
#include <WebController.h>

#include "./AddonResource.h"

class TestFixture : public Wt::WApplication
{
public:

    TestFixture(const Wt::WEnvironment& env)
    : Wt::WApplication(env),
    done_(false)
    {
    }

    void waitDone()
    {
        boost::mutex::scoped_lock guard(doneMutex_);

        while (!done_)
            doneCondition_.wait(guard);
    }

    void reset()
    {
        done_ = false;
    }

    void onDone(boost::system::error_code err, const Wt::Http::Message& m)
    {
        assert(WApplication::instance() == this);

        boost::mutex::scoped_lock guard(doneMutex_);

        err_ = err;
        message_ = m;

        if (err_)
            std::cerr << "Http client error: " << err_.message() << std::endl;
        else
        {
            std::cerr << "Http client result status: " << m.status() << std::endl;
            for (unsigned i = 0; i < m.headers().size(); ++i)
            {
                const Wt::Http::Message::Header& h = m.headers()[i];
                std::cerr << " " << h.name() << ": " << h.value() << std::endl;
            }
            std::cerr << " Body: -----" << std::endl;
            std::cerr << m.body();
            std::cerr << "-----" << std::endl;
        }

        done_ = true;
        doneCondition_.notify_one();
    }

private:
    bool done_;
    boost::condition doneCondition_;
    boost::mutex doneMutex_;
    

    boost::system::error_code err_;
    Wt::Http::Message message_;
};

BOOST_AUTO_TEST_SUITE (stringtest) // name of the test suite is stringtest

BOOST_AUTO_TEST_CASE (test1)
{
  std::string s;
  BOOST_CHECK(s.size() == 0);
}

BOOST_AUTO_TEST_CASE (test2)
{
  std::string s;
  s = "hello world";
  BOOST_REQUIRE_EQUAL ('h', s[0]); // basic test 
}

BOOST_AUTO_TEST_CASE(http_server_test)
{
    Wt::Test::WTestEnvironment environment("/","wt_config.xml");
    TestFixture app(environment);
    AddonResource addonResource;
     
    environment.server()->addResource(&addonResource, "/addons");
    environment.setHostName("localhost:8081");
    
    environment.server()->ioService().start();
    

    Wt::Http::Client *c = new Wt::Http::Client(&app);
    c->done().connect(boost::bind(&TestFixture::onDone, &app, _1, _2));

    std::string ok = "addons";
    
    if (c->get("http://localhost:8081/" + ok))
    {
        environment.endRequest();
        app.waitDone();
        environment.startRequest();
    }
    
    environment.server()->ioService().stop();
    environment.server()->ioService().start();
    app.reset();
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
