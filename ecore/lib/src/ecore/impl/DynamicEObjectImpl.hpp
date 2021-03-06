// *****************************************************************************
//
// This file is part of a MASA library or program.
// Refer to the included end-user license agreement for restrictions.
//
// Copyright (c) 2020 MASA Group
//
// *****************************************************************************

#ifndef ECORE_DYNAMIC_EOBJECT_HPP_
#define ECORE_DYNAMIC_EOBJECT_HPP_

#include "ecore/impl/EObjectImpl.hpp"

#include <vector>

namespace ecore::impl
{
    template <typename... I>
    class DynamicEObjectBase : public EObjectBase<I...>
    {
    public:
        DynamicEObjectBase();
        DynamicEObjectBase( const std::shared_ptr<EClass>& eClass );
        virtual ~DynamicEObjectBase();
        
        virtual std::shared_ptr<ecore::EClass> eClass() const;

        void setEClass( const std::shared_ptr<EClass>& eClass );

        std::shared_ptr<DynamicEObjectBase<I...>> getThisPtr() const;
        void setThisPtr( const std::shared_ptr<DynamicEObjectBase<I...>>& thisPtr );

    public:
        using EObject::eGet;
        using EObject::eSet;
        using EObject::eIsSet;
        using EObject::eUnset;
        using EObject::eInvoke;

    protected:
        virtual Any eGet( int featureID, bool resolve ) const;
        virtual bool eIsSet( int featureID ) const;
        virtual void eSet( int featureID, const Any& newValue );
        virtual void eUnset( int featureID );
        virtual Any eInvoke( int operationID, const std::shared_ptr<EList<Any>>& arguments );

    private:
        int eStaticFeatureCount() const;
        int eStaticOperationCount() const;
        int eDynamicFeatureID( const std::shared_ptr<EStructuralFeature>& eStructuralFeature ) const;
        std::shared_ptr<EStructuralFeature> eDynamicFeature( int dynamicFeatureID ) const;
        bool isBidirectional( const std::shared_ptr<EStructuralFeature>& eStructuralFeature ) const;
        bool isContainer( const std::shared_ptr<EStructuralFeature>& eStructuralFeature ) const;
        bool isContains( const std::shared_ptr<EStructuralFeature>& eStructuralFeature ) const;
        bool isBackReference( const std::shared_ptr<EStructuralFeature>& eStructuralFeature ) const;
        bool isProxy( const std::shared_ptr<EStructuralFeature>& eStructuralFeature ) const;

        void resizeProperties();
        std::shared_ptr<EList<std::shared_ptr<EObject>>> createList( const std::shared_ptr<EStructuralFeature>& eStructuralFeature ) const;

        using EObjectProxy = Proxy< std::shared_ptr<EObject> >;

    private:
        class FeaturesAdapter;
        std::unique_ptr<FeaturesAdapter> featuresAdapter_;

    protected:
        std::weak_ptr<EClass> eClass_;
        mutable std::vector< Any > properties_;
    };

    typedef DynamicEObjectBase<EObject> DynamicEObjectImpl;

}

#include "ecore/impl/DynamicEObjectImpl.inl"

#endif /* ECORE_DYNAMIC_EOBJECT_HPP_ */
