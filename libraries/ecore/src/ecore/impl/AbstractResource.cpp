#include "ecore/impl/AbstractResource.hpp"
#include "ecore/EAttribute.hpp"
#include "ecore/EClass.hpp"
#include "ecore/ECollectionView.hpp"
#include "ecore/ENotificationChain.hpp"
#include "ecore/ENotifyingList.hpp"
#include "ecore/EObject.hpp"
#include "ecore/EResourceSet.hpp"
#include "ecore/Stream.hpp"
#include "ecore/impl/AbstractENotifyingList.hpp"
#include "ecore/impl/AbstractNotification.hpp"
#include "ecore/impl/EcoreUtils.hpp"
#include "ecore/impl/StringUtils.hpp"
#include "ecore/impl/UriConverter.hpp"

#include <cctype>

using namespace ecore;
using namespace ecore::impl;

class AbstractResource::Notification : public AbstractNotification
{
public:
    Notification( const std::weak_ptr<ENotifier>& notifier,
                  EventType type,
                  int featureID,
                  const Any& oldValue,
                  const Any& newValue,
                  std::size_t position = NO_INDEX )
        : AbstractNotification( type, oldValue, newValue, position )
        , notifier_( notifier )
        , featureID_( featureID )
    {
    }

    virtual std::shared_ptr<ENotifier> getNotifier() const
    {
        return notifier_.lock();
    }

    virtual int getFeatureID() const
    {
        return featureID_;
    }

    virtual std::shared_ptr<EStructuralFeature> getFeature() const
    {
        return nullptr;
    }

private:
    std::weak_ptr<ENotifier> notifier_;
    int featureID_;
};

AbstractResource::AbstractResource()
{
}

AbstractResource::AbstractResource( const Uri& uri )
    : uri_( uri )
{
}

AbstractResource::~AbstractResource()
{
}

std::shared_ptr<EResourceSet> AbstractResource::getResourceSet() const
{
    return resourceSet_.lock();
}

const Uri& AbstractResource::getUri() const
{
    return uri_;
}

void AbstractResource::setUri( const Uri& uri )
{
    Uri oldUri = uri_;
    uri_ = uri;
    if( eNotificationRequired() )
        eNotify( std::make_shared<Notification>( thisPtr_, Notification::SET, RESOURCE__URI, oldUri, uri_ ) );
}

std::shared_ptr<EList<std::shared_ptr<EObject>>> AbstractResource::getContents() const
{
    return eContents_;
}

std::shared_ptr<const ECollectionView<std::shared_ptr<EObject>>> AbstractResource::getAllContents() const
{
    return std::make_shared<ECollectionView<std::shared_ptr<ecore::EObject>>>( getContents() );
}

std::shared_ptr<EObject> AbstractResource::getEObject( const std::string& uriFragment ) const
{
    auto id = uriFragment;
    auto size = uriFragment.size();
    if( !uriFragment.empty() )
    {
        if( uriFragment.at( 0 ) == '/' )
        {
            auto path = split( uriFragment, "/" );
            return getObjectByPath( path );
        }
        else if( uriFragment.at( size - 1 ) == '?' )
        {
            auto index = uriFragment.find_last_of( '?', size - 2 );
            if( index != std::string::npos )
                id = uriFragment.substr( 0, index );
        }
    }
    return getObjectByID( id );
}

std::shared_ptr<EObject> AbstractResource::getObjectByPath( const std::vector<std::string_view>& uriFragmentPath ) const
{
    auto eObject = getObjectForRootSegment( uriFragmentPath.empty() ? "" : std::string( uriFragmentPath.at( 0 ) ) );
    return std::shared_ptr<EObject>();
}

std::shared_ptr<EObject> AbstractResource::getObjectByID( const std::string& id ) const
{
    for( auto eObject : *getAllContents() )
    {
        auto objectID = EcoreUtils::getID( eObject );
        if( id == objectID )
            return eObject;
    }
    return std::shared_ptr<EObject>();
}

std::shared_ptr<EObject> ecore::impl::AbstractResource::getObjectForRootSegment( const std::string& rootSegment ) const
{
    int position = 0;
    if( !rootSegment.empty() )
    {
        if( rootSegment.at( 0 ) == '?' )
            return getObjectByID( rootSegment.substr( 1 ) );
        else
            position = std::stoi( rootSegment );
    }
    return position >= 0 && position < getContents()->size() ? getContents()->get( position ) : std::shared_ptr<EObject>();
}

std::shared_ptr<EObject> ecore::impl::AbstractResource::getObjectForFragmentSegment( const std::shared_ptr<EObject>& eObject,
                                                                                     const std::string& uriSegment ) const
{
    std::size_t index = std::string::npos;
    if( std::isdigit( uriSegment.back() ) )
    {
        index = uriSegment.find_last_of( '.' );
        if( index != std::string::npos )
        {
            auto position = std::stoi( uriSegment.substr( 0, index ) );
            auto eFeatureName = uriSegment.substr( 0, index );
            auto eFeature = getStructuralFeature( eObject, eFeatureName );
            auto value = eObject->eGet( eFeature );
            auto list = anyCast<std::shared_ptr<EList<std::shared_ptr<EObject>>>>( value );
            if( position < list->size() )
                return list->get( position );
        }
    }
    if( index == std::string::npos )
    {
        auto eFeature = getStructuralFeature( eObject, uriSegment );
        auto value = eObject->eGet( eFeature );
        return anyCast<std::shared_ptr<EObject>>( value );
    }
    return std::shared_ptr<EObject>();
}

std::shared_ptr<EStructuralFeature> AbstractResource::getStructuralFeature( const std::shared_ptr<EObject>& eObject,
                                                                            const std::string& name ) const
{
    auto eFeature = eObject->eClass()->getEStructuralFeature( name );
    if( !eFeature )
        throw std::runtime_error( "The feature " + name + " is not a valid feature" );
    return eFeature;
}

void AbstractResource::attached( const std::shared_ptr<EObject>& object )
{
}

void AbstractResource::detached( const std::shared_ptr<EObject>& object )
{
}

void AbstractResource::load()
{
    if( !isLoaded_ )
    {
        auto uriConverter = getUriConverter();
        auto is = uriConverter->createInputStream( uri_ );
        if( is )
            load( *is );
    }
}

void AbstractResource::load( std::istream& is )
{
    if( !isLoaded_ )
    {
        auto notifications = basicSetLoaded( true, nullptr );

        doLoad( is );

        if( notifications )
            notifications->dispatch();
    }
}

void AbstractResource::unload()
{
    if( isLoaded_ )
    {
        auto notifications = basicSetLoaded( false, nullptr );

        doUnload();

        if( notifications )
            notifications->dispatch();
    }
}

bool AbstractResource::isLoaded() const
{
    return isLoaded_;
}

void AbstractResource::save()
{
}

void AbstractResource::save( std::ostream& os )
{
}

std::shared_ptr<EList<std::shared_ptr<EDiagnostic>>> AbstractResource::getErrors() const
{
    return errors_;
}

std::shared_ptr<EList<std::shared_ptr<EDiagnostic>>> AbstractResource::getWarnings() const
{
    return warnings_;
}

std::shared_ptr<ENotificationChain> AbstractResource::basicSetLoaded( bool isLoaded, const std::shared_ptr<ENotificationChain>& msgs )
{
    auto notifications = msgs;
    bool oldLoaded = isLoaded_;
    isLoaded_ = isLoaded;
    if( eNotificationRequired() )
    {
        if( !notifications )
            notifications = std::make_shared<NotificationChain>();

        notifications->add( std::make_shared<Notification>( thisPtr_, Notification::SET, RESOURCE__IS_LOADED, oldLoaded, isLoaded_ ) );
    }
    return notifications;
}

std::shared_ptr<ENotificationChain> AbstractResource::basicSetResourceSet( const std::shared_ptr<EResourceSet> resourceSet,
                                                                           const std::shared_ptr<ENotificationChain>& msgs )
{
    auto notifications = msgs;
    auto oldAbstractResourceSet = resourceSet_.lock();
    if( oldAbstractResourceSet )
    {
        auto list = std::dynamic_pointer_cast<ENotifyingList<std::shared_ptr<EResource>>>( oldAbstractResourceSet->getResources() );
        _ASSERTE( list );
        notifications = list->add( thisPtr_.lock(), notifications );
    }
    resourceSet_ = resourceSet;
    if( eNotificationRequired() )
    {
        if( !notifications )
            notifications = std::make_shared<NotificationChain>();

        notifications->add(
            std::make_shared<Notification>( thisPtr_, Notification::SET, RESOURCE__RESOURCE_SET, oldAbstractResourceSet, resourceSet ) );
    }
    return notifications;
}

void AbstractResource::doUnload()
{
    eContents_->clear();
}

std::shared_ptr<EUriConverter> AbstractResource::getUriConverter() const
{
    auto resourceSet = resourceSet_.lock();
    if( resourceSet )
        resourceSet->getUriConverter();
    else
    {
        static std::shared_ptr<EUriConverter> defaultUriConverter = std::make_shared<UriConverter>();
        return defaultUriConverter;
    }

    return std::shared_ptr<EUriConverter>();
}

std::shared_ptr<EList<std::shared_ptr<EObject>>> AbstractResource::initContents()
{
    class ContentsEList : public AbstractENotifyingList<ENotifyingList<std::shared_ptr<EObject>>, std::shared_ptr<EObject>>
    {
    public:
        ContentsEList( AbstractResource& resource )
            : resource_( resource )
        {
        }

        virtual ~ContentsEList()
        {
        }

        virtual std::shared_ptr<ENotifier> getNotifier() const
        {
            return resource_.getThisPtr();
        }

        virtual int getFeatureID() const
        {
            return EResource::RESOURCE__CONTENTS;
        }

    protected:
        virtual std::shared_ptr<ENotificationChain> inverseAdd( const std::shared_ptr<EObject>& eObject,
                                                                const std::shared_ptr<ENotificationChain>& n ) const
        {
            auto notifications = n;
            notifications = eObject->eSetResource( resource_.getThisPtr(), notifications );
            resource_.attached( eObject );
            return notifications;
        }

        virtual std::shared_ptr<ENotificationChain> inverseRemove( const std::shared_ptr<EObject>& eObject,
                                                                   const std::shared_ptr<ENotificationChain>& n ) const
        {
            auto notifications = n;
            resource_.detached( eObject );
            notifications = eObject->eSetResource( nullptr, notifications );
            return notifications;
        }

    private:
        AbstractResource& resource_;
    };

    return std::make_shared<ContentsEList>( *this );
}

std::shared_ptr<EList<std::shared_ptr<EDiagnostic>>> AbstractResource::initDiagnostics()
{
    return std::make_shared<ArrayEList<std::shared_ptr<EDiagnostic>>>();
}
