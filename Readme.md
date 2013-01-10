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
* Calls are currently not threaded. They really should be (and will be soon, I promise)
* Project is currently mac only, but it will work on PC + Linux no problem (all calls are through OF's included version of Poco)
* Not tested with uploading video yet...

Setting Up
===========
* Download openframeworks
* Clone into openframeworks/addons
* Open an example project:
	* Upload: uploads to your Flickr (after authenticating) by dragging a photo into the window
