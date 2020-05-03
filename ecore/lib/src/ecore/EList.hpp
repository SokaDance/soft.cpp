// *****************************************************************************
//
// This file is part of a MASA library or program.
// Refer to the included end-user license agreement for restrictions.
//
// Copyright (c) 2018 MASA Group
//
// *****************************************************************************

#ifndef ECORE_ELIST_HPP_
#define ECORE_ELIST_HPP_

#include "ecore/Assert.hpp"
#include "ecore/EIterator.hpp"
#include "ecore/EIteratorAdapter.hpp"
#include <memory>
#include <stdexcept>

namespace ecore
{

    template <typename T>
    class EList : public std::enable_shared_from_this<EList<T>>
    {
    public:
        typedef typename T ValueType;

        virtual ~EList() = default;

        virtual bool add( const T& e ) = 0;

        virtual bool addAll( const EList<T>& l ) = 0;

        virtual bool add( std::size_t pos, const T& e ) = 0;

        virtual bool addAll( std::size_t pos, const EList<T>& l ) = 0;

        virtual void move( std::size_t newPos, const T& e ) = 0;

        virtual T move( std::size_t newPos, std::size_t oldPos ) = 0;

        virtual T get( std::size_t pos ) const = 0;

        virtual T set( std::size_t pos, const T& e ) = 0;

        virtual T remove( std::size_t pos ) = 0;

        virtual bool remove( const T& e ) = 0;

        virtual std::size_t size() const = 0;

        virtual void clear() = 0;

        virtual bool empty() const = 0;

        virtual std::shared_ptr<EIterator<T>> iterator() const;
        
        virtual bool contains( const T& e ) const;
       
        std::size_t indexOf( const T& e ) const;
        
        EIteratorAdapter<T> begin() const
        {
            return EIteratorAdapter<T>( iterator() );
        }

        EIteratorAdapter<T> end() const
        {
            return EIteratorAdapter<T>( );
        }

        /**
         * Allows treating an EList<T> as an EList<Q> (if T can be casted to Q)
         */
        template <typename Q>
        inline std::shared_ptr<EList<Q>> asEListOf()
        {
            return std::make_shared<detail::DelegateEList<Q, T>>( shared_from_this() );
        }

        template <typename Q>
        inline std::shared_ptr<const EList<Q>> asEListOf() const
        {
            return std::make_shared<detail::ConstDelegateEList<Q, T>>( shared_from_this() );
        }
    };

    template <typename T>
    auto inline begin( const std::shared_ptr<EList<T>>& l )
    {
        return EIteratorAdapter<T>( l->iterator() );
    }

    template <typename T>
    auto inline end( const std::shared_ptr<EList<T>>& l )
    {
        return EIteratorAdapter<T>();
    }


    template <typename T>
    bool operator==( const EList<T>& lhs, const EList<T>& rhs );

    template <typename T>
    bool operator!=( const EList<T>& lhs, const EList<T>& rhs );

} // namespace ecore

#include "ecore/EList.inl"

#endif /* ECORE_ELIST_HPP_ */
