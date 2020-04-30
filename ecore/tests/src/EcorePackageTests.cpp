#include <boost/test/unit_test.hpp>

#include "ecore/EcoreFactory.hpp"
#include "ecore/EcorePackage.hpp"

using namespace ecore;

BOOST_AUTO_TEST_SUITE( EcorePackageTests )

BOOST_AUTO_TEST_CASE( Constructor )
{
    BOOST_CHECK( EcorePackage::eInstance() );
}

BOOST_AUTO_TEST_CASE( Getters )
{
    BOOST_CHECK_EQUAL( EcorePackage::eInstance()->getEFactoryInstance(), EcoreFactory::eInstance() );
}

BOOST_AUTO_TEST_SUITE_END()
