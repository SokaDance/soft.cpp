// *****************************************************************************
//
// This file is part of a MASA library or program.
// Refer to the included end-user license agreement for restrictions.
//
// Copyright (c) 2018 MASA Group
//
// *****************************************************************************

#ifndef ECORE_ABSTRACTXMILOAD_HPP_
#define ECORE_ABSTRACTXMILOAD_HPP_

#include "ecore/impl/AbstractXMLLoad.hpp"

namespace ecore::impl
{
    class AbstractXMILoad : public AbstractXMLLoad
    {
    public:
        AbstractXMILoad( XMLResource& resource );

        virtual ~AbstractXMILoad();
    };

} // namespace ecore::impl

#endif
