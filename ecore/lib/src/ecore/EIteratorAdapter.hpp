// *****************************************************************************
//
// This file is part of a MASA library or program.
// Refer to the included end-user license agreement for restrictions.
//
// Copyright (c) 2018 MASA Group
//
// *****************************************************************************

#ifndef ECORE_EITERATORADAPTER_HPP_
#define ECORE_EITERATORADAPTER_HPP_

#include "ecore/EIterator.hpp"
#include <memory>

namespace ecore
{
    template <typename T>
    class EIteratorAdapter
    {
    public:
        using IteratorType = std::shared_ptr<EIterator<T>>;
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

    public:
        EIteratorAdapter()
        {
        }

        EIteratorAdapter( const IteratorType& it )
        {
            if( it && it->hasNext() )
            {
                it_ = it;
                value_ = it->next();
            }
        }

        T operator*() const
        {
            assertIterator();
            return value_;
        }

        EIteratorAdapter& operator++()
        {
            assertIterator();
            if( it_->hasNext() )
                value_ = it_->next();
            else
                it_ = nullptr;
            return *this;
        }

        bool operator==( const EIteratorAdapter& rhs ) const
        {
            return it_ == rhs.it_;
        }

        bool operator!=( const EIteratorAdapter& rhs ) const
        {
            return it_ != rhs.it_;
        }

    private:
        void assertIterator() const
        {
            if( !it_ )
                throw std::out_of_range( "iterator is out of range" );
        }

    private:
        IteratorType it_{};
        T value_{};
    };

    template <typename T>
    inline auto begin( const std::shared_ptr<EIterator<T>>& it )
    {
        return EIteratorAdapter<T>( it );
    }

    template <typename T>
    inline auto end( const std::shared_ptr<EIterator<T>>& )
    {
        return EIteratorAdapter<T>();
    }

} // namespace ecore

#endif