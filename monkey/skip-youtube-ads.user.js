// ==UserScript==
// @name         Auto Skip YouTube Ads
// @version      1.0.2
// @description  Speed up and skip YouTube ads automatically
// @author       jso8910, codiac-killer
// @match        *://*.youtube.com/*
// ==/UserScript==
// from https://www.reddit.com/r/qutebrowser/comments/ntl2ko/easy_youtube_adblocker_greasemonkey_script/

/*
setInterval(() => {
    const btn = document.querySelector('.videoAdUiSkipButton,.ytp-ad-skip-button-modern')
    if (btn) {
        btn.click()
    }
    const ad = [...document.querySelectorAll('.ad-showing')][0];
    if (ad) {
        document.querySelector('video').playbackRate = 16;
    }
}, 50)
*/

const removeAds = () => {
	// Get skip button and click it
	let btn = document.getElementsByClassName("ytp-ad-skip-button-modern ytp-button").item(0);
	if (btn) {
	    btn.click();
	}

	// (unskipable ads) If skip button didn't exist / was not clicked speed up video
	const ad = [...document.querySelectorAll('.ad-showing')][0];
	if (ad) {
	    // console.log("ad found");
	    // Speed up and mute
	    document.querySelector('video').playbackRate = 16;
	    document.querySelector('video').muted = true;
	// } else {
	//     console.log("ad not found");
	}

}

const main = new MutationObserver(() => {
    // call if ad watcher called late
    removeAds();
    const adContainer = document.getElementsByClassName("video-ads ytp-ad-module").item(0);
    if(adContainer) {
 	main.disconnect();
 	console.log("ad container found, observing...");
 	new MutationObserver(()=> removeAds()).observe(adContainer, {attributes: true, characterData: true, childList: true});
     } else {
 	console.log("adContainer not found!");
     }
})

// call in case page loads this script late and no mutations to observe
removeAds();
// look for ad container
main.observe(document.body, {attributes: false, characterData: false, childList: true, subtree: true});
