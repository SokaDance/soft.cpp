// *****************************************************************************
//
// This file is part of a MASA library or program.
// Refer to the included end-user license agreement for restrictions.
//
// Copyright (c) 2020 MASA Group
//
// *****************************************************************************

#ifndef ECORE_RESOURCE_SET_HPP_
#define ECORE_RESOURCE_SET_HPP_

#include "ecore/Exports.hpp"
#include "ecore/EResourceSet.hpp"
#include "ecore/URI.hpp"
#include "ecore/impl/BasicNotifier.hpp"
#include "ecore/impl/Lazy.hpp"

#include <optional>

namespace ecore::impl
{
    class ECORE_API ResourceSet : public virtual BasicNotifier<EResourceSet>
    {
    public:
        ResourceSet();

        virtual ~ResourceSet();

        virtual std::shared_ptr<EResource> createResource(const URI& uri);
        virtual std::shared_ptr<EList<std::shared_ptr<EResource>>> getResources() const;
        virtual std::shared_ptr<EResource> getResource(const URI& uri, bool loadOnDemand);

        virtual std::shared_ptr<EObject> getEObject(const URI& uri, bool loadOnDemand);

        virtual std::shared_ptr<URIConverter> getURIConverter() const;
        virtual void setURIConverter( const std::shared_ptr<URIConverter>& uriConverter );

        virtual std::shared_ptr<EResourceFactoryRegistry> getResourceFactoryRegistry() const;
        virtual void setResourceFactoryRegistry( const std::shared_ptr<EResourceFactoryRegistry>& resourceFactoryRegistry );

        virtual std::shared_ptr<EPackageRegistry> getPackageRegistry() const;
        virtual void setPackageRegistry(const std::shared_ptr<EPackageRegistry>& packageRegistry);

        virtual void setURIResourceMap(const std::unordered_map<URI, std::shared_ptr<EResource>>& uriMap);
        virtual std::unordered_map< URI, std::shared_ptr<EResource>> getURIResourceMap() const;

    private:
        std::shared_ptr<EList<std::shared_ptr<EResource>>> initResources();
     
    private:
        Lazy<std::shared_ptr<EList<std::shared_ptr<EResource>>>> resources_;
        Lazy<std::shared_ptr<URIConverter>> uriConverter_;
        Lazy<std::shared_ptr<EResourceFactoryRegistry>> resourceFactoryRegistry_;
        Lazy<std::shared_ptr<EPackageRegistry>> packageRegistry_;
        std::optional<std::unordered_map<URI, std::shared_ptr<EResource>>> uriResourceMap_;
    };

}

#endif
