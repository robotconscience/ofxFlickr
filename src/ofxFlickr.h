//
//  ofxFlickr.h
//  ChromatcCapture
//
//  Created by Brett Renfer on 12/1/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "ofMain.h"
#include "md5.h"
#include "ofxXmlSettings.h"

// Poco stuff
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPStreamFactory.h"
#include "Poco/Net/FilePartSource.h"
#include "Poco/StreamCopier.h"
#include "Poco/URIStreamOpener.h"
#include "Poco/Path.h"
#include "Poco/URI.h"
#include "Poco/Exception.h"

using namespace Poco::Net;
using namespace Poco;

namespace ofxFlickr {

    enum Permissions {
        FLICKR_READ = 0,
        FLICKR_WRITE,
        FLICKR_DELETE,
        FLICKR_NONE
    };
    
    enum Format {
        FLICKR_XML = 0,
        FLICKR_JSONP,
        FLICKR_JSON
    };
    
    enum Size {
        FLICKR_SIZE_DEFAULT = 0,
        FLICKR_SIZE_ORIGINAL,
        FLICKR_SIZE_S,
        FLICKR_SIZE_Q,
        FLICKR_SIZE_T,
        FLICKR_SIZE_M,
        FLICKR_SIZE_N,
        FLICKR_SIZE_Z,
        FLICKR_SIZE_C,
        FLICKR_SIZE_B,
    };
    
    enum MediaType {
        FLICKR_PHOTO = 0,
        FLICKR_VIDEO,
        FLICKR_UNKNOWN
    };
    
    /*
     From Flickr API:
     http://www.flickr.com/services/api/misc.urls.html
     
     default	medium, 500 on longest side
     o          original image, either a jpg, gif or png, depending on source format
     s          small square 75x75
     q          large square 150x150
     t          thumbnail, 100 on longest side
     m          small, 240 on longest side
     n          small, 320 on longest side
     z          medium 640, 640 on longest side
     c          medium 800, 800 on longest side 
     b          large, 1024 on longest side*

     */

    static const std::string api_base = "api.flickr.com";
    static const std::string auth_base = "http://flickr.com/services/auth/";
    
    // %a = API key, %b = perms, %c = frob, %d = api_sig
    static const std::string auth = "http://flickr.com/services/auth/?api_key=%a&perms=%b&frob=%c&api_sig=%d";
    
    class Media {
    public:
        
        Media();
        
        // stuff for making url
        string  id, farm, secret, server, originalsecret, originalformat;
        // more info
        string  title;
        string  isfavorite, license;
        int     rotation;
        
        void    loadFromXML( string XML );
        string  getURL( Size size = FLICKR_SIZE_DEFAULT );
        
        MediaType getType();
        
    private:
        
        // usually photo
        MediaType type;
    };
    
    static string getSizeURLParam( Size size );
    
    class API {
    public:
        
        API();
        
        /**
         * Authenticate with Flickr. Will open a browser asking you to allow access to your account.
         * One it does authenticate, will save a tiny XML file called flickr.xml to store your current auth_token
         * @param {std::string} api_key
         * @param {std::string} api_secret
         * @param {ofxFlickr::Permissions} perms
         */
        bool authenticate( string api_key, string api_secret, Permissions perms = FLICKR_WRITE );
        
        /**
         * Check API token
         * @param {std::string} api_key
         * @param {std::string} auth_token
         * @param {ofxFlickr::Permissions} perms
         */
        bool  checkAuthToken( string api_key, string auth_token, Permissions perms = FLICKR_WRITE );
        
        /**
         * Upload an image from disk
         * @returns {std::string} id of uploaded image. Use getMediaById to get ofxFlickr::Media, which includes the URL, etc.
         */
        string  upload( string image );
        
        /**
         * Get URL of photo by its ID (helpful after upload)
         * @param {std::string} id ID of photo you'd like to load into a ofxFlickr::Media object
         */
        Media & getMediaById( string id );
        
    private:
        bool                bAuthenticated;
        Permissions         currentPerms;
        ofURLFileLoader     dummyLoader; // this is needed because OF doesn't have a way to say "hey, we already made an HTTPStreamFactory!"
        map<string, Media>  loadedMedia;
        
        bool getAuthToken( Permissions perms );
        
        string apiSig( map<string, string> params );
        
        string makeAPICall( string method, map<string,string> args, Format format = FLICKR_XML, bool bSigned = false  );
        string buildAPICall( string method, map<string,string> args, Format format = FLICKR_XML, bool bSigned = false );
        string api_key, api_secret, frob, auth_token;
    };

}