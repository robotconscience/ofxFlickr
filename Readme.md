ofxFlickr
===========
An openFrameworks addon for interfacing with the Flickr API. Allows you to authenticate with users via OAuth, store a token (as XML), and perform authenticated calls (such as uploading images).

Current Status
===========
* Only supports a few calls:
	* uploads
	* flickr.photos.getInfo
	* flickr.auth.getFrob (part of authentication)
	* flickr.auth.checkToken (ditto)
* Logic is there to support almost the entire API, just needs to be written
* Calls are currently with threaded support (experimental). For now only upload threaded support. 
* Project is tested in mac, linux and windows (all calls are through OF's included version of Poco)
* Not tested with uploading video yet...

Setting Up
===========
* Download openframeworks
* Clone into openframeworks/addons
* Open an example project:
	* Upload: uploads to your Flickr (after authenticating) by dragging a photo into the window.
	* Upload in linux: uploads in linux not works by dragging, just click on the windows with the mouse. (because this issue: https://github.com/openframeworks/openFrameworks/issues/1862).

Tested in linux and windows (VS 2010) by @kalwalt.
