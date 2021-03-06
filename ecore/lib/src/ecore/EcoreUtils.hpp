// *****************************************************************************
//
// This file is part of a MASA library or program.
// Refer to the included end-user license agreement for restrictions.
//
// Copyright (c) 2020 MASA Group
//
// *****************************************************************************

#ifndef ECORE_ECORE_UTILS_HPP_
#define ECORE_ECORE_UTILS_HPP_

#include "ecore/Exports.hpp"
#include "ecore/Uri.hpp"

#include <memory>
#include <string>

namespace ecore
{
    class Any;
    class EObject;
    class EClass;
    class EDataType;
    class EResource;
    class EResourceSet;
    template <typename T>
    class EList;

    class ECORE_API EcoreUtils
    {
    public:
        static std::string getID( const std::shared_ptr<EObject>& eObject );

        static void setID( const std::shared_ptr<EObject>& eObject, const std::string& id );

        static std::string convertToString( const std::shared_ptr<EDataType>& eDataType, const Any& value );

        static Any createFromString( const std::shared_ptr<EDataType>& eDataType, const std::string& literal );

        static URI getURI( const std::shared_ptr<EObject>& eObject );

        static std::shared_ptr<EObject> getEObject( const std::shared_ptr<EObject>& rootEObject, const std::string& relativeFragmentPath );

        static std::shared_ptr<EObject> resolve( const std::shared_ptr<EObject>& proxy, const std::shared_ptr<EObject>& context );

        static std::shared_ptr<EObject> resolve( const std::shared_ptr<EObject>& proxy, const std::shared_ptr<EResource>& context );

        static std::shared_ptr<EObject> resolve( const std::shared_ptr<EObject>& proxy, const std::shared_ptr<EResourceSet>& context );

        static bool isAncestor( const std::shared_ptr<EObject>& ancestor, const std::shared_ptr<EObject>& object );

        // Determines if the class represented by eSuper object is either the same as, or is a superclass of, the class represented by the
        // specified eClass parameter. It returns true if so; otherwise it returns false.
        static bool isAssignableFrom( const std::shared_ptr<EClass>& eSuper, const std::shared_ptr<EClass>& eClass );

        static std::shared_ptr<EObject> copy( const std::shared_ptr<EObject>& eObject );

        static std::shared_ptr<EList<std::shared_ptr<EObject>>> copyAll( const std::shared_ptr<EList<std::shared_ptr<EObject>>>& eObjects );

        static bool equals( const std::shared_ptr<EObject>& eObject1, const std::shared_ptr<EObject>& eObject2 );

        static bool equals( const std::shared_ptr<EList<std::shared_ptr<EObject>>>& l1,
                            const std::shared_ptr<EList<std::shared_ptr<EObject>>>& l2 );

    private:
        static std::string getRelativeURIFragmentPath( const std::shared_ptr<EObject>& ancestor,
                                                       const std::shared_ptr<EObject>& descendant,
                                                       bool resolve );
    };

} // namespace ecore

#endif