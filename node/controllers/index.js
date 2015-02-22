'use strict';

var urls = require('../libs/urls');

var indexView = require('../views/index').indexView,
    trackerView = require('../views/index').trackerView,
    aboutView = require('../views/index').aboutView,
    faqView = require('../views/index').faqView,
    privacyView = require('../views/index').privacyView,
    aboutView = require('../views/index').aboutView,
    downloadView = require('../views/index').downloadView,
    twitchView = require('../views/twitch').twitchView;

module.exports = function (router) {

    var index = urls.add('/', router);
    index.get('/', 'index', indexView.asView);

    /*
        /download/
        creates a zip file based on the Release folder
        automatically generates the settings.ini file
    */
    var download = urls.add('/download/', router);
    download.get('/download/', 'download', downloadView.asView);
    
    var faq = urls.add('/faq/', router);
    faq.get('/faq/', 'faq', faqView.asView);
    
    var privacy = urls.add('/privacy/', router);
    privacy.get('/privacy/', 'privacy', privacyView.asView);
    
    var about = urls.add('/about/', router);
    about.get('/about/', 'about', aboutView.asView);
    var twitch = urls.add('/twitch/', router);
    /*
        /twitch/
        used to log in and log out with twitch
     */
    twitch.get('/twitch/', 'twitch-redirect', function(req, res) {
        /* change this to class based view if we actually do something more complicated */
        res.redirect(urls.reverse('index')); 
    });
    twitch.get('/twitch/:action', 'twitch-action', twitchView.asView);

    /*
        /:display_name/
        shows tracked information about the specified twitch display_name 
     */
    index.get('/:display_name/', 'tracker-detail', trackerView.asView);
    
};
