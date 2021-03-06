#include <boost/test/unit_test.hpp>

#include "ecore/ECollectionView.hpp"
#include "ecore/Stream.hpp"
#include "ecore/impl/ImmutableArrayEList.hpp"

#include "ecore/tests/MockEObject.hpp"

using namespace ecore;
using namespace ecore::impl;
using namespace ecore::tests;

BOOST_AUTO_TEST_SUITE( ECollectionViewTests )

BOOST_AUTO_TEST_CASE( Constructor )
{
    auto emptyList = std::make_shared<ImmutableArrayEList<std::shared_ptr<EObject>>>();
    auto mockObject = std::make_shared<MockEObject>();
    MOCK_EXPECT( mockObject->eContents ).returns( emptyList );
    ECollectionView<std::shared_ptr<EObject>> view( mockObject );
}

BOOST_AUTO_TEST_CASE( Iterator )
{
    auto emptyList = std::make_shared<ImmutableArrayEList<std::shared_ptr<EObject>>>();
    auto mockObject = std::make_shared<MockEObject>();
    auto mockChild1 = std::make_shared<MockEObject>();
    auto mockGrandChild1 = std::make_shared<MockEObject>();
    auto mockGrandChild2 = std::make_shared<MockEObject>();
    auto mockChild2 = std::make_shared<MockEObject>();
    MOCK_EXPECT( mockObject->eContents )
        .returns( std::make_shared<ImmutableArrayEList<std::shared_ptr<EObject>>>(
            std::initializer_list<std::shared_ptr<EObject>>{mockChild1, mockChild2} ) );
    MOCK_EXPECT( mockChild1->eContents )
        .returns( std::make_shared<ImmutableArrayEList<std::shared_ptr<EObject>>>(
            std::initializer_list<std::shared_ptr<EObject>>{mockGrandChild1, mockGrandChild2} ) );
    MOCK_EXPECT( mockGrandChild1->eContents ).returns( emptyList );
    MOCK_EXPECT( mockGrandChild2->eContents ).returns( emptyList );
    MOCK_EXPECT( mockChild2->eContents ).returns( emptyList );

    ECollectionView<std::shared_ptr<EObject>> view( mockObject );
    std::vector<std::shared_ptr<EObject>> v;
    for( auto o : view )
        v.push_back( o );
    BOOST_CHECK_EQUAL( v, std::vector<std::shared_ptr<EObject>>( {mockChild1, mockGrandChild1, mockGrandChild2, mockChild2} ) );
}

BOOST_AUTO_TEST_SUITE_END()