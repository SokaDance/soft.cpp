#include <boost/test/execution_monitor.hpp>
#include <boost/test/unit_test.hpp>

#include "ecore/Stream.hpp"
#include "ecore/impl/BasicEList.hpp"
#include "ecore/impl/ImmutableEList.hpp"
#include <Windows.h>

using namespace ecore;
using namespace ecore::impl;

namespace
{
    struct no_debugger
    {
        boost::test_tools::assertion_result operator()( boost::unit_test::test_unit_id )
        {
            return !IsDebuggerPresent();
        }
    };
} // namespace

namespace std
{
    template <typename T>
    ostream& operator<<( ostream& os, const std::vector<T>& v )
    {
        return print_container( os, v );
    }

    template <typename T>
    bool operator==( const EList<T>& lhs, const std::vector<T>& rhs )
    {
        return lhs.size() == rhs.size() && std::equal( lhs.begin(), lhs.end(), rhs.begin() );
    }
} // namespace std

BOOST_AUTO_TEST_SUITE( BasicEListTests )

BOOST_AUTO_TEST_CASE( Constructor_Default )
{
    BOOST_CHECK_NO_THROW( BasicEList<int> list; );
}

BOOST_AUTO_TEST_CASE( Constructor_Initializer )
{
    BasicEList<int> list = {1, 2};
    BOOST_CHECK_EQUAL( list, std::vector<int>( {1, 2} ) );
}

BOOST_AUTO_TEST_CASE( Empty )
{
    {
        BasicEList<int> list;
        BOOST_CHECK( list.empty() );
    }
    {
        BasicEList<int> list = {1};
        BOOST_CHECK( !list.empty() );
    }
}

BOOST_AUTO_TEST_CASE( Size )
{
    {
        BasicEList<int> list;
        BOOST_CHECK_EQUAL( list.size(), 0 );
    }
    {
        BasicEList<int> list = {1, 2};
        BOOST_CHECK_EQUAL( list.size(), 2 );
    }
}

BOOST_AUTO_TEST_CASE( Get )
{
    BasicEList<int> list = {1, 2};
    BOOST_CHECK_EQUAL( list.get( 0 ), 1 );
    BOOST_CHECK_EQUAL( list.get( 1 ), 2 );
}

BOOST_AUTO_TEST_CASE( Iterator_Empty )
{
    BasicEList<int> list;
    // stl iterator
    BOOST_CHECK( list.begin() == list.end() );

    // list iteraror
    auto it = list.iterator();
    BOOST_CHECK( !it->hasNext() );

    // range loop
    bool called = false;
    for( int i : list )
        called = true;
    BOOST_CHECK( !called );
}

BOOST_AUTO_TEST_CASE( Iterators )
{
    // init
    BasicEList<int> list;
    for( int i = 0; i < 10; ++i )
        list.add( i );

    // stl iterator
    BOOST_CHECK( list.begin() != list.end() );

    // list iteraror
    auto it = list.iterator();
    BOOST_CHECK( it->hasNext() );
    BOOST_CHECK_EQUAL( it->next() , 0 );

    // range loop
    int test = 0;
    for( int i : list )
        BOOST_CHECK_EQUAL( i, test++ );
}

BOOST_AUTO_TEST_CASE( IndexOf )
{
    BasicEList<int> list;
    list.add( 1 );
    list.add( 2 );
    BOOST_CHECK_EQUAL( list.indexOf( 1 ), 0 );
    BOOST_CHECK_EQUAL( list.indexOf( 2 ), 1 );
    BOOST_CHECK_EQUAL( list.indexOf( 3 ), -1 );
}

BOOST_AUTO_TEST_CASE( Add )
{
    BasicEList<int> list;
    BOOST_CHECK_EQUAL( list.add( 1 ), true );
    BOOST_CHECK_EQUAL( list.add( 2 ), true );
    BOOST_CHECK_EQUAL( list.size(), 2 );
    BOOST_CHECK_EQUAL( list.get( 0 ), 1 );
    BOOST_CHECK_EQUAL( list.get( 1 ), 2 );
}

#ifdef _DEBUG
BOOST_AUTO_TEST_CASE( Add_InvalidIndex, *boost::unit_test::precondition( no_debugger() ) )
{
    BasicEList<int> list;
    BOOST_REQUIRE_THROW( list.add( 1, 0 ), boost::execution_exception );
}
#endif

BOOST_AUTO_TEST_CASE( Add_Index )
{
    BasicEList<int> list;
    BOOST_CHECK( list.add( 0, 1 ) );
    BOOST_CHECK( list.add( 0, 2 ) );
    BOOST_CHECK_EQUAL( list.size(), 2 );
    BOOST_CHECK_EQUAL( list.get( 0 ), 2 );
    BOOST_CHECK_EQUAL( list.get( 1 ), 1 );
}

BOOST_AUTO_TEST_CASE( AddAll )
{
    {
        BasicEList<int> list;
        ImmutableEList<int> other( std::initializer_list<int>{1, 2} );
        BOOST_CHECK( list.addAll( other ) );
        BOOST_CHECK_EQUAL( list, std::vector<int>( {1, 2} ) );
    }
    {
        BasicEList<int> list( std::initializer_list<int>{1, 2} );
        ImmutableEList<int> other( std::initializer_list<int>{3, 4} );
        BOOST_CHECK( list.addAll( other ) );
        BOOST_CHECK_EQUAL( list, std::vector<int>( {1, 2, 3, 4} ) );
    }
}

BOOST_AUTO_TEST_CASE( AddAll_Index )
{
    {
        BasicEList<int> list( std::initializer_list<int>{1, 2} );
        ImmutableEList<int> other( std::initializer_list<int>{3, 4} );
        BOOST_CHECK( list.addAll( 0, other ) );
        BOOST_CHECK_EQUAL( list, std::vector<int>( {3, 4, 1, 2} ) );
    }
    {
        BasicEList<int> list( std::initializer_list<int>{1, 2} );
        ImmutableEList<int> other( std::initializer_list<int>{3, 4} );
        BOOST_CHECK( list.addAll( 1, other ) );
        BOOST_CHECK_EQUAL( list, std::vector<int>( {1, 3, 4, 2} ) );
    }
    {
        BasicEList<int> list( std::initializer_list<int>{1, 2} );
        ImmutableEList<int> other( std::initializer_list<int>{3, 4} );
        BOOST_CHECK( list.addAll( 2, other ) );
        BOOST_CHECK_EQUAL( list, std::vector<int>( {1, 2, 3, 4} ) );
    }
}

BOOST_AUTO_TEST_CASE( Unique_Add )
{
    BasicEList<int, true> list;
    BOOST_CHECK( list.add( 1 ) );
    BOOST_CHECK( !list.add( 1 ) );
}

BOOST_AUTO_TEST_CASE( Unique_Add_Index )
{
    BasicEList<int, true> list;
    list.add( 0, 1 );
    list.add( 0, 2 );
    BOOST_CHECK_EQUAL( list, std::vector<int>( {2, 1} ) );
}

#ifdef _DEBUG
BOOST_AUTO_TEST_CASE( Unique_Add_Index_InvalidElement, *boost::unit_test::precondition( no_debugger() ) )
{
    BasicEList<int, true> list;
    list.add( 0, 1 );
    BOOST_REQUIRE_THROW( list.add( 1, 1 ), boost::execution_exception );
}
#endif // DEBUG

BOOST_AUTO_TEST_CASE( Unique_AddAll )
{
    BasicEList<int, true> list( std::initializer_list<int>{1, 2} );
    ImmutableEList<int> other( std::initializer_list<int>{2, 3} );
    BOOST_CHECK( list.addAll( other ) );
    BOOST_CHECK_EQUAL( list, std::vector<int>( {1, 2, 3} ) );
}

BOOST_AUTO_TEST_CASE( Unique_AddAll_Index )
{
    {
        BasicEList<int, true> list = {1, 2};
        ImmutableEList<int> other = {2, 3};
        BOOST_CHECK( list.addAll( 0, other ) );
        BOOST_CHECK_EQUAL( list, std::vector<int>( {3, 1, 2} ) );
    }
    {
        BasicEList<int, true> list( std::initializer_list<int>{1, 2} );
        ImmutableEList<int> other( std::initializer_list<int>{2, 3} );
        BOOST_CHECK( list.addAll( 1, other ) );
        BOOST_CHECK_EQUAL( list, std::vector<int>( {1, 3, 2} ) );
    }
    {
        BasicEList<int, true> list( std::initializer_list<int>{1, 2} );
        ImmutableEList<int> other( std::initializer_list<int>{2, 3} );
        BOOST_CHECK( list.addAll( 2, other ) );
        BOOST_CHECK_EQUAL( list, std::vector<int>( {1, 2, 3} ) );
    }
}

#ifdef _DEBUG
BOOST_AUTO_TEST_CASE( Remove_InvalidIndex, *boost::unit_test::precondition( no_debugger() ) )
{
    BasicEList<int> list;
    BOOST_REQUIRE_THROW( list.remove( std::size_t{0} ), boost::execution_exception );
}
#endif

BOOST_AUTO_TEST_CASE( Remove_Index )
{
    BasicEList<int> list = {1, 2};
    int old = list.remove( std::size_t{0} );
    BOOST_CHECK_EQUAL( old, 1 );
    BOOST_CHECK_EQUAL( list.size(), 1 );
}

#ifdef _DEBUG
BOOST_AUTO_TEST_CASE( Set_InvalidIndex, *boost::unit_test::precondition( no_debugger() ) )
{
    BasicEList<int> list = {1, 2};
    BOOST_REQUIRE_THROW( list.set( 2, 3 ), boost::execution_exception );
}
#endif

BOOST_AUTO_TEST_CASE( Set )
{
    BasicEList<int> list = {1, 2};
    BOOST_CHECK_EQUAL( list.set( 0, 3 ), 1 );
    BOOST_CHECK_EQUAL( list.get( 0 ), 3 );
}

BOOST_AUTO_TEST_CASE( Set_Unique )
{
    BasicEList<int, true> list = {1, 2};
    BOOST_CHECK_EQUAL( list.set( 0, 3 ), 1 );
    BOOST_CHECK_EQUAL( list.set( 0, 3 ), 3 );
}

#ifdef _DEBUG
BOOST_AUTO_TEST_CASE( Set_UniqueInvalid, *boost::unit_test::precondition( no_debugger() ) )
{
    BasicEList<int, true> list = {1, 2};
    BOOST_CHECK_THROW( list.set( 1, 1 ), boost::execution_exception );
}
#endif

BOOST_AUTO_TEST_CASE( Clear )
{
    BasicEList<int> list = {1, 2};
    list.clear();
    BOOST_CHECK( list.empty() );
}

BOOST_AUTO_TEST_CASE( Move )
{
    BasicEList<int> list = {1, 2, 3, 4, 5};
    list.move( std::size_t{3}, std::size_t{2} );
    BOOST_CHECK_EQUAL( list, std::vector<int>( {1, 2, 4, 3, 5} ) );
}

BOOST_AUTO_TEST_CASE( Delegate_SharedPtr )
{
    class A
    {
    public:
        A() = default;

        virtual ~A() = default;
    };

    class B : public A
    {
    public:
        B() = default;

        virtual ~B() = default;
    };

    auto init = std::initializer_list<std::shared_ptr<B>>( {std::make_shared<B>(), std::make_shared<B>()} );
    auto list = std::make_shared<BasicEList<std::shared_ptr<B>>>( init );
    auto delegate = list->asEListOf<std::shared_ptr<A>>();
    BOOST_CHECK_EQUAL( delegate->size(), 2 );

    auto newB = std::make_shared<B>();
    BOOST_CHECK( delegate->add( newB ) );
    BOOST_CHECK_EQUAL( list->size(), 3 );
    BOOST_CHECK_EQUAL( list->get( 2 ), newB );
}

BOOST_AUTO_TEST_CASE( Delegate_Any )
{
    class A
    {
    public:
        A() = default;

        virtual ~A() = default;
    };

    auto init = std::initializer_list<std::shared_ptr<A>>( {std::make_shared<A>(), std::make_shared<A>()} );
    auto list = std::make_shared<BasicEList<std::shared_ptr<A>>>( init );
    auto delegate = list->asEListOf<Any>();

    auto newA = std::make_shared<A>();
    BOOST_CHECK( delegate->add( newA ) );
    BOOST_CHECK_EQUAL( list->size(), 3 );
    BOOST_CHECK_EQUAL( list->get( 2 ), newA );
    BOOST_CHECK_EQUAL( delegate->get( 2 ), newA );
}

BOOST_AUTO_TEST_SUITE_END()
