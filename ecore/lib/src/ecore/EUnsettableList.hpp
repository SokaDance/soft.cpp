// *****************************************************************************
//
// This file is part of a MASA library or program.
// Refer to the included end-user license agreement for restrictions.
//
// Copyright (c) 2018 MASA Group
//
// *****************************************************************************

#ifndef ECORE_EUNSETTABLELIST_HPP_
#define ECORE_EUNSETTABLELIST_HPP_

#include "ecore/EObjectList.hpp"

namespace ecore
{
    template <typename T>
    class EUnsettableList : public EObjectList<T>
    {
    public:
        virtual ~EUnsettableList() {}

        virtual bool isSet() const = 0;

        virtual void unset() = 0;
    };
}



#endif /* ECORE_EUNSETTABLELIST_HPP_ */
