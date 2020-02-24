// *****************************************************************************
//
// This file is part of a MASA library or program.
// Refer to the included end-user license agreement for restrictions.
//
// Copyright (c) 2018 MASA Group
//
// *****************************************************************************

#ifndef ECORE_URI_HPP_
#error This file may only be included from URI.h
#endif

#include "URI.hpp"
#include <functional>
#include <tuple>
#include <array>

namespace ecore
{
    namespace detail
    {
        using URITuple = std::tuple<const std::string&,
                                    const std::string&,
                                    const std::string&,
                                    const std::string&,
                                    uint16_t,
                                    const std::string&,
                                    const std::string&,
                                    const std::string&>;

        inline URITuple as_tuple( const URI& k )
        {
            return URITuple( k.getScheme(),
                             k.getUsername(),
                             k.getPassword(),
                             k.getHost(),
                             k.getPort(),
                             k.getPath(),
                             k.getQuery(),
                             k.getFragment() );
        }

        // Map from character code to escape mode:
        // 0 = pass through
        // 1 = unused
        // 2 = pass through in PATH mode
        // 3 = space, replace with '+' in QUERY mode
        // 4 = percent-encode
        extern const std::array<unsigned char, 256> uriEscapeTable;


        // Map from the character code to the hex value, or 16 if invalid hex char.
        extern const std::array<unsigned char, 256> hexTable;

    } // namespace detail


    inline const std::string& URI::getScheme() const
    {
        return scheme_;
    }
    
    inline const std::string& URI::getUsername() const
    {
        return username_;
    }
    
    inline const std::string& URI::getPassword() const
    {
        return password_;
    }

    inline const std::string& URI::getHost() const
    {
        return host_;
    }

    inline uint16_t URI::getPort() const
    {
        return port_;
    }

    inline const std::string& URI::getPath() const
    {
        return path_;
    }

    inline const std::string& URI::getQuery() const
    {
        return query_;
    }

    inline const std::string& URI::getFragment() const
    {
        return fragment_;
    }

    inline void URI::setFragment(const std::string& fragment) {
        fragment_ = fragment;
    }

    inline bool URI::operator==( const URI& other ) const
    {
        return ecore::detail::as_tuple( *this ) == ecore::detail::as_tuple( other );
    }

    inline bool URI::operator!=( const URI& other ) const
    {
        return !operator==( other );
    }

    inline bool URI::isAbsolute() const
    {
        return !scheme_.empty();
    }

    inline bool URI::isOpaque() const
    {
        return path_.empty();
    }

    inline bool URI::isEmpty() const
    {
        return *this == URI();
    }

    template <typename InputIterator, typename OutputIterator>
    void uriEscape( InputIterator first, InputIterator last, OutputIterator out, URIEscapeMode mode )
    {
        static const char hexValues[] = "0123456789abcdef";
        char esc[3];
        esc[0] = '%';
        auto p = first;
        auto l = p;
        // We advance over runs of passthrough characters and copy them in one go;
        unsigned char minEncode = static_cast<unsigned char>( mode );
        while( p != last )
        {
            char c = *p;
            unsigned char v = static_cast<unsigned char>( c );
            unsigned char discriminator = detail::uriEscapeTable[v];
            if(  discriminator <= minEncode )
            {
                ++p;
            }
            else if( mode == URIEscapeMode::QUERY && discriminator == 3 )
            {
                std::copy( l, p, out );
                out++ = '+';
                ++p;
                l = p;
            }
            else
            {
                std::copy( l, p, out );
                esc[1] = hexValues[v >> 4];
                esc[2] = hexValues[v & 0x0f];
                std::copy( std::begin(esc) , std::end(esc), out );
                ++p;
                l = p;
            }
        }
        std::copy( l, p, out );
    }

    template <typename StringInput, typename StringOutput>
    StringOutput uriEscape( const StringInput& str, URIEscapeMode mode )
    {
        StringOutput out;
        uriEscape( std::begin( str ), std::end( str ), std::back_inserter( out ), mode );
        return out;
    }

    template <typename InputIterator, typename OutputIterator>
    void uriUnescape( InputIterator first, InputIterator last, OutputIterator out, URIEscapeMode mode )
    {
        auto p = first;
        auto l = p;
        // We advance over runs of passthrough characters and copy them in one go;
        // this is faster than calling push_back repeatedly.
        while( p != last )
        {
            char c = *p;
            switch( c )
            {
            case '%':
            {
                if( std::distance( p, last ) < 3 )
                    throw std::invalid_argument( "incomplete percent encode sequence" );
             
                auto h1 = detail::hexTable[static_cast<unsigned char>( p[1] )];
                auto h2 = detail::hexTable[static_cast<unsigned char>( p[2] )];
                if( h1 == 16 || h2 == 16 )
                    throw std::invalid_argument( "invalid percent encode sequence" );
                
                std::copy( l, p, out );
                out++ = ( h1 << 4 ) | h2;
                p += 3;
                l = p;
                break;
            }
            case '+':
                if( mode == URIEscapeMode::QUERY )
                {
                    std::copy( l, p, out );
                    out++ = ' ';
                    ++p;
                    l = p;
                    break;
                }
            default:
                ++p;
                break;
            }
        }
        std::copy( l, p, out );
    }

    template <typename StringInput, typename StringOutput>
    StringOutput uriUnescape( const StringInput& str, URIEscapeMode mode )
    {
        StringOutput out;
        uriUnescape( std::begin( str ), std::end( str ), std::back_inserter( out ), mode );
        return out;
    }

} // namespace ecore

namespace std
{

    template <>
    struct equal_to<ecore::URI>
    {
        bool operator()( const ecore::URI& a, const ecore::URI& b ) const
        {
            return ecore::detail::as_tuple( a ) == ecore::detail::as_tuple( b );
        }
    };

    template <>
    struct hash<ecore::URI>
    {
        std::size_t operator()(const ecore::URI& uri) const
        {
            return std::hash<std::string>()(uri.toString());
        }
    };


} // namespace std
