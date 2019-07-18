// *****************************************************************************
//
// This file is part of a MASA library or program.
// Refer to the included end-user license agreement for restrictions.
//
// Copyright (c) 2018 MASA Group
//
// *****************************************************************************

#ifndef ECORE_EOBJECTELIST_HPP_
#define ECORE_EOBJECTELIST_HPP_

#include "ecore/ENotifyingList.hpp"
#include "ecore/EUnsettableList.hpp"
#include "ecore/impl/AbstractArrayEList.hpp"
#include "ecore/impl/AbstractEObjectEList.hpp"
#include "ecore/impl/Proxy.hpp"


namespace ecore::impl
{

    template <typename T, bool containement = false, bool inverse = false, bool opposite = false, bool proxies = false, bool unset = false >
    class EObjectEList : public AbstractEObjectEList< typename std::conditional<unset, EUnsettableList<T>, ENotifyingList<T>>::type
                                                    , typename std::conditional<proxies, Proxy<T>, T>::type
                                                    , containement
                                                    , inverse
                                                    , opposite>
    {
        template <class...> struct null_v : std::integral_constant<int, 0> {};
    public:
        using EList<T>::add;
        using ENotifyingList<T>::add;

        template <long = null_v<std::enable_if_t< !proxies >>::value >
        EObjectEList( const std::weak_ptr<EObject>& owner, int featureID, int inverseFeatureID = -1 )
            : AbstractEObjectEList( owner, featureID, inverseFeatureID )
        {
        }

        template <int = null_v<std::enable_if_t< proxies >>::value >
        EObjectEList( const std::weak_ptr<EObject>& owner, int featureID, int inverseFeatureID = -1 )
            : AbstractEObjectEList( []( const Proxy<T>& p ) { return p.get(); }
                                  , [=]( const T& v ) { return Proxy<T>(owner,featureID_,v); } 
                                  , owner
                                  , featureID
                                  , inverseFeatureID)
        {
        }

        virtual ~EObjectEList()
        {
        }
    };
}



#endif /* ECORE_EOBJECTELIST_HPP_ */
