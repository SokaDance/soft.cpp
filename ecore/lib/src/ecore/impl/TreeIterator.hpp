// *****************************************************************************
//
// This file is part of a MASA library or program.
// Refer to the included end-user license agreement for restrictions.
//
// Copyright (c) 2018 MASA Group
//
// *****************************************************************************

#ifndef ECORE_IMPL_TREEITERATOR_HPP_
#define ECORE_IMPL_TREEITERATOR_HPP_

#include "ecore/EIterator.hpp"

#include <functional>
#include <memory>
#include <stack>
#include <stdexcept>
#include <type_traits>

namespace ecore::impl
{

    template <typename T>
    class TreeIterator : public EIterator<T>
    {
    public:
        TreeIterator( const std::shared_ptr<EIterator<T>>& it, std::function<std::shared_ptr<EIterator<T>>( const T& )> getChildren )
            : getChildren_( getChildren )
        {
            stack_.push( it );
        }

        virtual ~TreeIterator()
        {
        }

        virtual bool hasNext()
        {
            return !stack_.empty() && stack_.top()->hasNext();
        }

        virtual T next()
        {
            if( stack_.empty() )
                throw std::out_of_range( "iterator out of range" );

            auto current = stack_.top();
            auto result = current->next();
            auto it = std::invoke( getChildren_, result );
            if( it && it->hasNext() )
                stack_.push( it );
            else
            {
                while( !current->hasNext() )
                {
                    stack_.pop();
                    if( stack_.empty() )
                        break;
                    current = stack_.top();
                }
            }
            return result;
        }

    private:
        std::function<std::shared_ptr<EIterator<T>>( const T& )> getChildren_;
        std::stack<std::shared_ptr<EIterator<T>>> stack_;
    };
} // namespace ecore::impl

#endif /* ECORE_ETREEITERATOR_HPP_ */
