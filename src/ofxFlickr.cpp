//
//  ofxFlickr.cpp
//  ChromatcCapture
//
//  Created by Brett Renfer on 12/1/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "ofxFlickr.h"

namespace ofxFlickr {
    
#pragma mark ofxFlickr::Media
    
    Media::Media(){
        id = "";
        farm = "";
        secret = "";
        server = "";
        originalsecret = "";
        originalformat = "";
        type = FLICKR_UNKNOWN;
        rotation = 0;
    }
    
    //--------------------------------------------------------------
    void Media::loadFromXML( string XML ){
        cout << XML << endl;
        ofxXmlSettings xml;
        xml.loadFromBuffer(XML);
        xml.pushTag("rsp");{
            id = xml.getAttribute("photo", "id", "");
            farm = xml.getAttribute("photo", "farm", "");
            secret = xml.getAttribute("photo", "secret", "");
            server = xml.getAttribute("photo", "server", "");
            originalsecret = xml.getAttribute("photo", "originalsecret", "");
            originalformat = xml.getAttribute("photo", "originalformat", "");
            
            string t = xml.getAttribute("photo", "media", "");
            if ( t == "photo"){
                type = FLICKR_PHOTO;
            } else if ( t == "video"){
                type = FLICKR_VIDEO;
            } else {
                type = FLICKR_UNKNOWN;
            }
        } xml.popTag();
        
        cout << getURL() << endl;
        // to-do: other stuff
    }
    
    //--------------------------------------------------------------
    string Media::getURL( Size size ){
        if ( size == FLICKR_SIZE_DEFAULT ){
            return "http://farm" + farm +".staticflickr.com/" + server +"/" + id +"_" + secret +".jpg";
        } else if ( size == FLICKR_SIZE_ORIGINAL ){
            return "http://farm" + farm +".staticflickr.com/" + server +"/" + id +"_" + originalsecret +"_o." + originalformat;
        } else {
            return "http://farm" + farm +".staticflickr.com/" + server +"/" + id +"_" + secret +"_" + getSizeURLParam( size ) +".jpg";
        }
    }
    
    
    //--------------------------------------------------------------
    MediaType Media::getType(){
        return type;
    }
    
    //--------------------------------------------------------------
    string getSizeURLParam( Size size ){
        switch (size) {
            case FLICKR_SIZE_S:
                return "s";
                break;
            case FLICKR_SIZE_Q:
                return "q";
                break;
            case FLICKR_SIZE_T:
                return "t";
                break;
            case FLICKR_SIZE_M:
                return "m";
                break;
            case FLICKR_SIZE_N:
                return "n";
                break;
            case FLICKR_SIZE_Z:
                return "z";
                break;
            case FLICKR_SIZE_C:
                return "c";
                break;
            case FLICKR_SIZE_B:
                return "b";
                break;
            default:
                return "o";
                break;
        }
    }
    
#pragma mark ofxFlickr::API
    
    //--------------------------------------------------------------
    API::API() :
    bAuthenticated(false),
    currentPerms(FLICKR_NONE)
    {
        
    };
    
    //--------------------------------------------------------------
    bool API::authenticate( string _api_key, string _api_secret, Permissions perms  ){
        api_key = _api_key;
        api_secret = _api_secret;
        
        // try to load from xml
        ofxXmlSettings xml;
        bool bLoaded = xml.loadFile("flickr.xml");
        
        if ( bLoaded ){
            xml.pushTag("settings");{
                auth_token = xml.getValue("token", "");
                if (auth_token == "" ){
                    bLoaded = false;
                }
            }; xml.popTag();
        }
        
        // not loaded, definitely need to get one
        if ( !bLoaded ) {
            bAuthenticated = getAuthToken( perms );
        } else {
            // loaded, might not be valid! check to see if we need to load
            if ( !checkAuthToken( api_key, auth_token, perms ) ){
                bAuthenticated = getAuthToken( perms );
            } else {
                bAuthenticated = true;
            }
        }
        
        if ( bAuthenticated ) {
            currentPerms = perms;
        } else {
            currentPerms = FLICKR_NONE;
        }
        
        return bAuthenticated;
    }
    
    //--------------------------------------------------------------
    bool API::checkAuthToken( string api_key, string auth_token, Permissions perms ){
        // build call
        map<string,string> args;
        args["api_key"]     = api_key;
        args["auth_token"]  = auth_token;
        
        // get frob
        string result = makeAPICall( "flickr.auth.checkToken", args, FLICKR_XML, true );
        ofxXmlSettings xml;
        xml.loadFromBuffer(result);
        string status = xml.getAttribute("rsp", "stat", "");
        
        bool bNeedNewToken  = true;
        string perm         = "";
        
        if (status == "ok"){
            xml.pushTag("rsp");{
                xml.pushTag("auth");{
                    perm = xml.getValue("perms", "");
                }; xml.popTag();
            }; xml.popTag();
            
            cout << perm << endl;
            
            // check if we have permissions
            switch (perms) {
                case FLICKR_WRITE:
                    if ( perm == "write" || perm == "delete") bNeedNewToken = false;
                    break;
                case FLICKR_READ:
                    if ( perm == "read") bNeedNewToken = false;
                    break;
                case FLICKR_DELETE:
                    if ( perm == "delete") bNeedNewToken = false;
                    break;
            }
        } else {
            cout << result << endl;
        }
        return !bNeedNewToken;
    }
    
    //--------------------------------------------------------------
    bool API::getAuthToken( Permissions perms ){
        // build call
        map<string,string> args;
        args["api_key"] = api_key;
        
        // get frob
        string result = makeAPICall( "flickr.auth.getFrob", args, FLICKR_XML, true );
        
        ofxXmlSettings xml;
        xml.loadFromBuffer(result);
        xml.pushTag("rsp");{
            frob = xml.getValue("frob", "");
        }; xml.popTag();
        
        // authenticate
        
        // %a = API key, %b = perms, %c = frob, %d = api_sig
        string authURL = auth;
        ofStringReplace(authURL, "%a", api_key);
        string perm = "write";
        
        switch (perms) {
            case FLICKR_WRITE:
                perm = "write";
                break;
            case FLICKR_READ:
                perm = "read";
                break;
            case FLICKR_DELETE:
                perm = "delete";
                break;
        }
        map<string,string> toEncode;
        toEncode["api_key"] = api_key;
        toEncode["perms"]   = perm;
        toEncode["frob"]    = frob;
        
        ofStringReplace(authURL, "%b", perm);
        ofStringReplace(authURL, "%c", frob);
        ofStringReplace(authURL, "%d", apiSig(toEncode));
        
        // this part is weird! ofLaunchBrowser has a tiny bug
#ifdef TARGET_OSX
		string commandStr = "open '"+authURL +"'";
		system(commandStr.c_str());
#else
        ofLaunchBrowser(authURL);
#endif
        bool bValidToken = false;
        int  numSeconds  = 0;
        int  secondsWait = 2;
        
        for( numSeconds; numSeconds<30; numSeconds+=secondsWait ){
            map<string,string> auth_args;
            auth_args["api_key"]    = api_key;
            auth_args["frob"]       = frob;
            
            // get frob
            string auth_result = makeAPICall( "flickr.auth.getToken", auth_args, FLICKR_XML, true );
            
            xml.loadFromBuffer(auth_result);
            xml.pushTag("rsp"); {
                xml.pushTag("auth"); {
                    auth_token = xml.getValue("token", "");
                } xml.popTag();
            } xml.popTag();
            
            bValidToken = !( auth_token == "" );
            
            if ( bValidToken ) break;
            numSeconds += secondsWait;
            sleep(1);
        }
        
        if ( !bValidToken ){
            ofLogError( "OAuth didn't succeed. Maybe you took too long?");
            return false;
        }
        
        // save auth token to XML for safe keeping
        
        ofxXmlSettings toSave;
        toSave.addTag("settings");
        toSave.pushTag("settings");{
            toSave.addValue("token", auth_token);
        }; toSave.popTag();
        toSave.saveFile("flickr.xml");
        
        return true;
    }
    
    //--------------------------------------------------------------
    string API::apiSig( map<string, string> params ){
        string toEncode = api_secret;
        
        // concatenate errbody
        map<string,string>::iterator it = params.begin();
        for ( it; it != params.end(); ++it ){
            toEncode += it->first + it->second;
        }
        
        return md5( toEncode );
    }
    
    //-------------------------------------------------------------
    string API::makeAPICall( string method, map<string,string> args, Format format, bool bSigned  ){
        string path     = buildAPICall( method, args, format, bSigned );
        string result   = "";
        
        try
        {
            // Get REST style xml as string from flickr
            std::auto_ptr<std::istream> pStr(URIStreamOpener::defaultOpener().open( "http://" + api_base + path ));
            StreamCopier::copyToString(*pStr.get(), result);
        }
        catch (Exception &ex)
        {
            cerr << ex.displayText() << endl;
        }
        return result;
    }
    
    //-------------------------------------------------------------
    string API::buildAPICall( string method, map<string,string> args, Format format, bool bSigned ){
        string call = "/services/rest/?method=" + method;
        
        switch ( format ) {
            case FLICKR_XML:
                args["format"]          = "rest";
                break;
                
            case FLICKR_JSON:
                args["format"]          = "json";
                args["nojsoncallback"]  = "1";
                break;
                
            case FLICKR_JSONP:
                args["format"]          = "json";
                break;
        }
        
        map<string,string>::iterator it = args.begin();
        for ( it; it != args.end(); ++it ){
            call += "&" + it->first + "=" + it->second;
        }
        
        if ( bSigned ){
            args["method"] = method;
            call += "&api_sig=" + apiSig( args );
        }
        
        return call;
    }
    
    //-------------------------------------------------------------
    string API::upload( string image ){
        
        if ( !bAuthenticated ){
            ofLogWarning( "Not authenticated! Please call authenticate() with proper api key and secret" );
            return "";
        } else if ( currentPerms != FLICKR_WRITE ){
            ofLogWarning( "You do not have proper permissions to upload! Please call authenticate() with permissions of ofxFlickr::FLICKR_WRITE" );
            return "";
        }
        
        map<string,string> args;
        args["api_key"] = api_key;
        args["auth_token"] = auth_token;
        
        string result;
        
        FilePartSource * fps = new FilePartSource(image, "image/jpeg");
        
        try
        {
            
            // prepare session
            HTTPClientSession session( api_base );
            HTTPRequest req(HTTPRequest::HTTP_POST, "/services/upload/", HTTPMessage::HTTP_1_0);
            req.setContentType("multipart/form-data");
            
            // setup form
            HTMLForm form;
            form.set("api_key", api_key);
            form.set("auth_token", auth_token);
            form.set("api_sig", apiSig( args ));
            form.setEncoding(HTMLForm::ENCODING_MULTIPART);
            form.addPart("photo", fps);
            form.prepareSubmit(req);
            
            std::ostringstream oszMessage;
            form.write(oszMessage);
            std::string szMessage = oszMessage.str();
            
            req.setContentLength((int) szMessage.length() );
            
            //session.setKeepAlive(true);
            
            // send form
            ostream & out = session.sendRequest(req) << szMessage;
            
            // get response
            HTTPResponse res;
            cout << res.getStatus() << " " << res.getReason() << endl;
            
            // print response
            istream &is = session.receiveResponse(res);
            StreamCopier::copyToString(is, result);
        }
        catch (Exception &ex)
        {
            cerr << "error? " + ex.displayText() <<endl;
        }
        
        
        string photoid;
        
        ofxXmlSettings xml;
        xml.loadFromBuffer(result);
        xml.pushTag("rsp");{
            photoid = xml.getValue("photoid", "");
        }; xml.popTag();
        
        return photoid;
    }
    
    //-------------------------------------------------------------
    Media & API::getMediaById( string id ){
        if ( loadedMedia.count(id) < 1 ){
            // build call
            map<string,string> args;
            args["api_key"]     = api_key;
            args["photo_id"]    = id;
            
            // get frob
            string result = makeAPICall(  "flickr.photos.getInfo", args, FLICKR_XML );
            loadedMedia[id] = Media();
            loadedMedia[id].loadFromXML( result );
        }
        return loadedMedia[id];
    }
}
