// ==UserScript==
// @name        Block Site
// @namespace   Monkey Script
// @match       http*://*.wenxuecity.com/*
// @match       http*://*.qidian.com/*
// @match       http*://*.twitter.com/*
// @match       http*://*.zhihu.com/*
// @match       http*://*.booklink.me/*
// @match       http*://*.bridgebase.com/*
// @run-at      document-start
// @grant       none
// @version     1.0
// @author      Yuantai Du
// @description 11/27/2023, 11:50:46 AM
// ==/UserScript==

// original: https://greasyfork.org/en/scripts/25448-block-sites/code

(function () {
	'use strict';

	var fullBlock = true;

	if (fullBlock) {
		window.history.back();
		return;
	}

	document.body.style.visibility = "hidden";

	document.addEventListener("DOMContentLoaded", function () {
		var oldBody = document.body.innerHTML;
		document.body.innerHTML = '<div id="NewContent">Connected. <a href="javascript:void(0)">Restore</a></div>';
		document.body.style.visibility = "visible";

		var style = document.createElement('style');
		style.textContent = `
        #NewContent {
        height: 300px;
        margin: 50px;
        padding: 50px;
        font-size:20pt;
        text-align:center;
        }`;
		document.head.append(style);

		document.querySelector("body a").addEventListener("click", function () {
			document.body.innerHTML = oldBody;
		});

	});

})();
