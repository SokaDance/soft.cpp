// *****************************************************************************
//
// This file is part of a MASA library or program.
// Refer to the included end-user license agreement for restrictions.
//
// Copyright (c) 2018 MASA Group
//
// *****************************************************************************

#ifndef ECORE_EITERATOR_HPP_
#define ECORE_EITERATOR_HPP_


namespace ecore
{
    template <typename T>
    class EIterator
    {
    public:
        virtual ~EIterator() = default;

        virtual bool hasNext() = 0;

        virtual T next() = 0;
    };
}

#endif