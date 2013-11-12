/*
 * File:   newtestclass.cpp
 * Author: tsa
 *
 * Created on 12 nov. 2013, 14:42:57
 */

#include "newtestclass.h"


CPPUNIT_TEST_SUITE_REGISTRATION(newtestclass);

newtestclass::newtestclass()
{
}

newtestclass::~newtestclass()
{
}

void newtestclass::setUp()
{
}

void newtestclass::tearDown()
{
}

void newtestclass::testMethod()
{
    CPPUNIT_ASSERT(true);
}

void newtestclass::testFailedMethod()
{
    CPPUNIT_ASSERT(false);
}

