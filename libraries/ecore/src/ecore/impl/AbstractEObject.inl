// *****************************************************************************
//
// This file is part of a MASA library or program.
// Refer to the included end-user license agreement for restrictions.
//
// Copyright (c) 2020 MASA Group
//
// *****************************************************************************

#ifndef ECORE_ABSTRACTEOBJECT_HPP_
#error This file may only be included from AbstractEObject.hpp
#endif

#include "ecore/Any.hpp"
#include "ecore/AnyCast.hpp"
#include "ecore/Constants.hpp"
#include "ecore/EAdapter.hpp"
#include "ecore/EClass.hpp"
#include "ecore/ECollectionView.hpp"
#include "ecore/ENotifyingList.hpp"
#include "ecore/EOperation.hpp"
#include "ecore/EReference.hpp"
#include "ecore/EResource.hpp"
#include "ecore/EStructuralFeature.hpp"
#include "ecore/EcorePackage.hpp"
#include "ecore/EcoreUtils.hpp"
#include "ecore/impl/Notification.hpp"

#include <sstream>
#include <string>
#include "AbstractEObject.hpp"

namespace ecore::impl
{
    template <typename...I>
    AbstractEObject<I...>::AbstractEObject()
        : eContainer_()
        , eContainerFeatureID_( -1 )
    {
    }

    template <typename... I>
    AbstractEObject<I...>::~AbstractEObject()
    {
    }

    template<typename ...I>
    inline const impl::EObjectInternal& AbstractEObject<I...>::getInternal() const
    {
        return *this;
    }

    template<typename ...I>
    inline impl::EObjectInternal& AbstractEObject<I...>::getInternal()
    {
        return *this;
    }

    template <typename... I>
    std::shared_ptr<const ECollectionView<std::shared_ptr<ecore::EObject>>> AbstractEObject<I...>::eAllContents() const
    {
        return std::make_shared<ECollectionView<std::shared_ptr<ecore::EObject>>>( eContents() );
    }

    template <typename... I>
    std::shared_ptr<const EList<std::shared_ptr<ecore::EObject>>> AbstractEObject<I...>::eContents() const
    {
        return eContentsList( eClass()->getEContainments() );
    }

    template <typename... I>
    std::shared_ptr<const EList<std::shared_ptr<ecore::EObject>>> AbstractEObject<I...>::eCrossReferences() const
    {
        return eContentsList( eClass()->getECrossReferences() );
    }

    template <typename... I>
    std::shared_ptr<const EList<std::shared_ptr<EObject>>> AbstractEObject<I...>::eContentsList(
        const std::shared_ptr<const EList<std::shared_ptr<ecore::EReference>>>& refs ) const
    {
        std::vector<std::shared_ptr<EObject>> contents;
        for( auto ref : *refs )
        {
            if( eIsSet( ref ) )
            {
                auto value = eGet( ref );
                if( ref->isMany() )
                {
                    auto l = anyListCast<std::shared_ptr<EObject>>( value );
                    std::copy( l->begin(), l->end(), std::back_inserter( contents ) );
                }
                else if( !value.empty() )
                {
                    auto object = anyObjectCast<std::shared_ptr<EObject>>( value );
                    if( object )
                        contents.push_back( object );
                }
            }
        }
        return std::make_shared<ImmutableEList<std::shared_ptr<EObject>>>( std::move( contents ) );
    }

    template <typename... I>
    std::shared_ptr<ecore::EClass> AbstractEObject<I...>::eClass() const
    {
        return eStaticClass();
    }

    template <typename... I>
    std::shared_ptr<EClass> AbstractEObject<I...>::eStaticClass() const
    {
        return EcorePackage::eInstance()->getEObject();
    }

    template <typename... I>
    std::shared_ptr<ecore::EObject> AbstractEObject<I...>::eContainer() const
    {
        return eContainer_.lock();
    }

    template <typename... I>
    int AbstractEObject<I...>::eContainerFeatureID() const
    {
        return eContainerFeatureID_;
    }

    template <typename... I>
    std::shared_ptr<EObject> AbstractEObject<I...>::eObjectForFragmentSegment( const std::string& uriSegment ) const
    {
        std::size_t index = std::string::npos;
        if( !uriSegment.empty() && std::isdigit( uriSegment.back() ) )
        {
            index = uriSegment.find_last_of( '.' );
            if( index != std::string::npos )
            {
                auto position = std::stoi( uriSegment.substr( index + 1 ) );
                auto eFeatureName = uriSegment.substr( 0, index );
                auto eFeature = eStructuralFeature( eFeatureName );
                auto value = eGet( eFeature );
                auto list = anyListCast<std::shared_ptr<EObject>>( value );
                if( position < list->size() )
                    return list->get( position );
            }
        }
        if( index == std::string::npos )
        {
            auto eFeature = eStructuralFeature( uriSegment );
            auto value = eGet( eFeature );
            return anyCast<std::shared_ptr<EObject>>( value );
        }
        return std::shared_ptr<EObject>();
    }

    template <typename... I>
    std::string AbstractEObject<I...>::eURIFragmentSegment( const std::shared_ptr<EStructuralFeature>& eFeature,
                                                   const std::shared_ptr<EObject>& eObject ) const
    {
        std::stringstream s;
        s << "@";
        s << eFeature->getName();
        if( eFeature->isMany() )
        {
            auto v = eGet( eFeature, false );
            auto l = anyListCast<std::shared_ptr<EObject>>( v );
            auto index = l->indexOf( eObject );
            s << ".";
            s << index;
        }
        return s.str();
    }

    template <typename... I>
    std::shared_ptr<EStructuralFeature> AbstractEObject<I...>::eStructuralFeature( const std::string& name ) const
    {
        auto eFeature = eClass()->getEStructuralFeature( name );
        if( !eFeature )
            throw std::runtime_error( "The feature " + name + " is not a valid feature" );
        return eFeature;
    }

    template <typename... I>
    std::shared_ptr<ecore::EStructuralFeature> AbstractEObject<I...>::eContainingFeature() const
    {
        auto eContainer = eContainer_.lock();
        if( eContainer )
        {
            return eContainerFeatureID_ <= EOPPOSITE_FEATURE_BASE
                       ? eContainer->eClass()->getEStructuralFeature( EOPPOSITE_FEATURE_BASE - eContainerFeatureID_ )
                       : std::dynamic_pointer_cast<EReference>( eClass()->getEStructuralFeature( eContainerFeatureID_ ) )->getEOpposite();
        }
        return std::shared_ptr<ecore::EStructuralFeature>();
    }

    template <typename... I>
    std::shared_ptr<ecore::EReference> AbstractEObject<I...>::eContainmentFeature() const
    {
        return eContainmentFeature( getThisPtr(), eContainer_.lock(), eContainerFeatureID_ );
    }

    template <typename... I>
    std::shared_ptr<EReference> AbstractEObject<I...>::eContainmentFeature( const std::shared_ptr<EObject>& eObject,
                                                                   const std::shared_ptr<EObject>& eContainer,
                                                                   int eContainerFeatureID )
    {
        if( eContainer )
        {
            if( eContainerFeatureID <= EOPPOSITE_FEATURE_BASE )
            {
                auto eFeature = eContainer->eClass()->getEStructuralFeature( EOPPOSITE_FEATURE_BASE - eContainerFeatureID );
                if( auto eReference = std::dynamic_pointer_cast<EReference>( eFeature ) )
                    return eReference;
            }
            else
            {
                auto eFeature = eObject->eClass()->getEStructuralFeature( eContainerFeatureID );
                if( auto eReference = std::dynamic_pointer_cast<EReference>( eFeature ) )
                    return eReference->getEOpposite();
            }
            throw "The containment feature could not be located";
        }
        return std::shared_ptr<EReference>();
    }

    template <typename... I>
    bool AbstractEObject<I...>::eIsProxy() const
    {
        return static_cast<bool>( eProxyURI_ );
    }

    template <typename... I>
    std::shared_ptr<EResource> AbstractEObject<I...>::eResource() const
    {
        auto eResource = eResource_.lock();
        if( !eResource )
        {
            auto eContainer = eContainer_.lock();
            if( eContainer )
                eResource = eContainer->eResource();
        }
        return eResource;
    }

    template <typename... I>
    std::shared_ptr<EResource> AbstractEObject<I...>::eDirectResource() const
    {
        return eResource_.lock();
    }

    template <typename... I>
    void AbstractEObject<I...>::eSetDirectResource( const std::shared_ptr<EResource>& resource )
    {
        eResource_ = resource;
    }

    template <typename... I>
    std::shared_ptr<ENotificationChain> AbstractEObject<I...>::eSetResource( const std::shared_ptr<EResource>& newResource,
                                                                    const std::shared_ptr<ENotificationChain>& n )
    {
        auto notifications = n;
        auto oldResource = eResource_.lock();
        auto thisPtr = thisPtr_.lock();
        if( oldResource && newResource )
        {
            auto list = std::dynamic_pointer_cast<ENotifyingList<std::shared_ptr<EObject>>>( oldResource->getContents() );
            _ASSERTE( list );
            notifications = list->remove( thisPtr, notifications );

            oldResource->detached( getThisPtr() );
        }

        auto eContainer = eContainer_.lock();
        if( eContainer )
        {
            if( eContainmentFeature()->isResolveProxies() )
            {
                auto oldContainerResource = eContainer->eResource();
                if( oldContainerResource )
                {
                    // If we're not setting a new resource, attach it to the old container's resource.
                    if( !newResource )
                        oldContainerResource->attached( thisPtr );
                    // If we didn't detach it from an old resource already, detach it from the old container's resource.
                    else if( !oldResource )
                        oldContainerResource->detached( thisPtr );
                }
            }
            else
            {
                notifications = eBasicRemoveFromContainer( notifications );
                notifications = eBasicSetContainer( nullptr, -1, notifications );
            }
        }
        eSetDirectResource( newResource );
        return notifications;
    }

    template <typename... I>
    Any AbstractEObject<I...>::eGet( const std::shared_ptr<EStructuralFeature>& feature ) const
    {
        return eGet( feature, true );
    }

    template <typename... I>
    Any AbstractEObject<I...>::eGet( const std::shared_ptr<EStructuralFeature>& feature, bool resolve ) const
    {
        return eGet( feature, resolve, true );
    }

    template <typename... I>
    int AbstractEObject<I...>::eDerivedStructuralFeatureID( const std::shared_ptr<EStructuralFeature>& eStructuralFeature ) const
    {
        VERIFYN( eClass()->getEAllStructuralFeatures()->contains( eStructuralFeature ),
                 "The feature '%s' is not a valid feature",
                 eStructuralFeature->getName().c_str() );
        return eStructuralFeature->getFeatureID();
    }

    template <typename... I>
    int AbstractEObject<I...>::eDerivedOperationID( const std::shared_ptr<EOperation>& eOperation ) const
    {
        VERIFYN( eClass()->getEAllOperations()->contains( eOperation ),
                 "The operation '%s' is not a valid operation",
                 eOperation->getName().c_str() );
        return eOperation->getOperationID();
    }

    template <typename... I>
    Any AbstractEObject<I...>::eGet( const std::shared_ptr<EStructuralFeature>& eFeature, bool resolve, bool coreType ) const
    {
        int featureID = eDerivedStructuralFeatureID( eFeature );
        if( featureID >= 0 )
            return eGet( featureID, resolve, coreType );
        throw "The feature '" + eFeature->getName() + "' is not a valid feature";
    }

    template <typename... I>
    Any AbstractEObject<I...>::eGet( int featureID, bool resolve, bool coreType ) const
    {
        std::shared_ptr<EStructuralFeature> eFeature = eClass()->getEStructuralFeature( featureID );
        VERIFYN( eFeature, "Invalid featureID: %i ", featureID );
        return Any();
    }

    template <typename... I>
    bool AbstractEObject<I...>::eIsSet( const std::shared_ptr<EStructuralFeature>& eFeature ) const
    {
        int featureID = eDerivedStructuralFeatureID( eFeature );
        if( featureID >= 0 )
            return eIsSet( featureID );
        throw "The feature '" + eFeature->getName() + "' is not a valid feature";
    }

    template <typename... I>
    bool AbstractEObject<I...>::eIsSet( int featureID ) const
    {
        std::shared_ptr<EStructuralFeature> eFeature = eClass()->getEStructuralFeature( featureID );
        VERIFYN( eFeature, "Invalid featureID: %i ", featureID );
        return false;
    }

    template <typename... I>
    void AbstractEObject<I...>::eSet( const std::shared_ptr<EStructuralFeature>& eFeature, const Any& newValue )
    {
        int featureID = eDerivedStructuralFeatureID( eFeature );
        if( featureID >= 0 )
            eSet( featureID, newValue );
        else
            throw "The feature '" + eFeature->getName() + "' is not a valid feature";
    }

    template <typename... I>
    void AbstractEObject<I...>::eSet( int featureID, const Any& newValue )
    {
        std::shared_ptr<EStructuralFeature> eFeature = eClass()->getEStructuralFeature( featureID );
        VERIFYN( eFeature, "Invalid featureID: %i ", featureID );
    }

    template <typename... I>
    void AbstractEObject<I...>::eUnset( const std::shared_ptr<EStructuralFeature>& eFeature )
    {
        int featureID = eDerivedStructuralFeatureID( eFeature );
        if( featureID >= 0 )
            eUnset( featureID );
        else
            throw "The feature '" + eFeature->getName() + "' is not a valid feature";
    }

    template <typename... I>
    void AbstractEObject<I...>::eUnset( int featureID )
    {
        std::shared_ptr<EStructuralFeature> eFeature = eClass()->getEStructuralFeature( featureID );
        VERIFYN( eFeature, "Invalid featureID: %i ", featureID );
    }

    template <typename... I>
    Any AbstractEObject<I...>::eInvoke( const std::shared_ptr<EOperation>& eOperation, const std::shared_ptr<EList<Any>>& arguments )
    {
        int operationID = eDerivedOperationID( eOperation );
        if( operationID >= 0 )
            return eInvoke( operationID, arguments );
        throw "The operation '" + eOperation->getName() + "' is not a valid operation";
    }

    template <typename... I>
    Any AbstractEObject<I...>::eInvoke( int operationID, const std::shared_ptr<EList<Any>>& arguments )
    {
        std::shared_ptr<EOperation> eOperation = eClass()->getEOperation( operationID );
        VERIFYN( eOperation, "Invalid operationID: %i ", operationID );
        return Any();
    }

    template <typename... I>
    std::shared_ptr<ENotificationChain> AbstractEObject<I...>::eBasicInverseAdd( const std::shared_ptr<EObject>& otherEnd,
                                                                        int featureID,
                                                                        const std::shared_ptr<ENotificationChain>& notifications )
    {
        return notifications;
    }

    template <typename... I>
    std::shared_ptr<ENotificationChain> AbstractEObject<I...>::eBasicInverseRemove( const std::shared_ptr<EObject>& otherEnd,
                                                                           int featureID,
                                                                           const std::shared_ptr<ENotificationChain>& notifications )
    {
        return notifications;
    }

    template <typename... I>
    std::shared_ptr<ENotificationChain> AbstractEObject<I...>::eInverseAdd( const std::shared_ptr<EObject>& otherEnd,
                                                                   int featureID,
                                                                   const std::shared_ptr<ENotificationChain>& n )
    {
        auto notifications = n;
        if( featureID >= 0 )
            return eBasicInverseAdd( otherEnd, featureID, notifications );
        else
        {
            notifications = eBasicRemoveFromContainer( notifications );
            return eBasicSetContainer( otherEnd, featureID, notifications );
        }
    }

    template <typename... I>
    std::shared_ptr<ENotificationChain> AbstractEObject<I...>::eInverseRemove( const std::shared_ptr<EObject>& otherEnd,
                                                                      int featureID,
                                                                      const std::shared_ptr<ENotificationChain>& notifications )
    {
        if( featureID >= 0 )
            return eBasicInverseRemove( otherEnd, featureID, notifications );
        else
            return eBasicSetContainer( nullptr, featureID, notifications );
    }

    template <typename... I>
    URI AbstractEObject<I...>::eProxyURI() const
    {
        return eProxyURI_.value_or( URI() );
    }

    template <typename... I>
    void AbstractEObject<I...>::eSetProxyURI( const URI& uri )
    {
        eProxyURI_ = uri;
    }

    template <typename... I>
    std::shared_ptr<EObject> AbstractEObject<I...>::eResolveProxy( const std::shared_ptr<EObject>& proxy ) const
    {
        return EcoreUtils::resolve( proxy, getThisPtr() );
    }

    template <typename... I>
    std::shared_ptr<ENotificationChain> AbstractEObject<I...>::eBasicSetContainer( const std::shared_ptr<EObject>& newContainer,
                                                                          int newContainerFeatureID,
                                                                          const std::shared_ptr<ENotificationChain>& n )
    {
        auto notifications = n;
        auto thisPtr = thisPtr_.lock();
        auto oldContainer = eContainer_.lock();
        auto oldResource = eDirectResource();

        // resource
        std::shared_ptr<EResource> newResource;
        if( oldResource )
        {
            if( newContainer && !eContainmentFeature( thisPtr, newContainer, newContainerFeatureID ) )
            {
                auto list = std::dynamic_pointer_cast<ENotifyingList<std::shared_ptr<EObject>>>( oldResource->getContents() );
                _ASSERTE( list );
                notifications = list->remove( thisPtr, notifications );
                eSetDirectResource( nullptr );
                newResource = newContainer->eResource();
            }
            else
                oldResource = nullptr;
        }
        else
        {
            if( oldContainer )
                oldResource = oldContainer->eResource();

            if( newContainer )
                newResource = newContainer->eResource();
        }

        if( oldResource && oldResource != newResource )
            oldResource->detached( thisPtr );

        if( newResource && newResource != oldResource )
            newResource->attached( thisPtr );

        // basic set
        int oldContainerFeatureID = eContainerFeatureID_;
        eContainer_ = newContainer;
        eContainerFeatureID_ = newContainerFeatureID;

        // notification
        if( eNotificationRequired() )
        {
            if( oldContainer && oldContainerFeatureID >= 0 && oldContainerFeatureID != newContainerFeatureID )
            {
                auto notification = std::make_shared<Notification>(
                    getThisPtr(), ENotification::SET, oldContainerFeatureID, oldContainer, std::shared_ptr<EObject>() );
                if( notifications )
                    notifications->add( notification );
                else
                    notifications = notification;
            }
            if( newContainerFeatureID >= 0 )
            {
                auto notification = std::make_shared<Notification>(
                    getThisPtr(),
                    ENotification::SET,
                    newContainerFeatureID,
                    oldContainerFeatureID == newContainerFeatureID ? oldContainer : std::shared_ptr<EObject>(),
                    newContainer );
                if( notifications )
                    notifications->add( notification );
                else
                    notifications = notification;
            }
        }
        return notifications;
    }

    template <typename... I>
    std::shared_ptr<ENotificationChain> AbstractEObject<I...>::eBasicRemoveFromContainer( const std::shared_ptr<ENotificationChain>& notifications )
    {
        if( eContainerFeatureID_ >= 0 )
            return eBasicRemoveFromContainerFeature( notifications );
        else
        {
            auto eContainer = eContainer_.lock();
            if( eContainer )
                return eContainer->getInternal().eInverseRemove( getThisPtr(), EOPPOSITE_FEATURE_BASE - eContainerFeatureID_, notifications );
        }
        return notifications;
    }

    template <typename... I>
    std::shared_ptr<ENotificationChain> AbstractEObject<I...>::eBasicRemoveFromContainerFeature(
        const std::shared_ptr<ENotificationChain>& notifications )
    {
        auto reference = std::dynamic_pointer_cast<EReference>( eClass()->getEStructuralFeature( eContainerFeatureID_ ) );
        if( reference )
        {
            auto inverseFeature = reference->getEOpposite();
            auto container = eContainer_.lock();
            if( container && inverseFeature )
                return container->getInternal().eInverseRemove( getThisPtr(), inverseFeature->getFeatureID(), notifications );
        }
        return notifications;
    }

} // namespace ecore::impl