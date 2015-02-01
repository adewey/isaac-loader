'use strict';

var urls = require('../libs/urls');

var indexView = require('../views/index').indexView,
    trackerView = require('../views/index').trackerView,
    actionView = require('../views/index').actionView,
    twitchView = require('../views/twitch').twitchView;

module.exports = function (router) {

    var index = urls.add('/', router);
    index.get('/', 'index', indexView.asView);

    
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
    /*
        /api/:stream_key/:action/
        allows users to update their tracked data (through rest api)
     */
    index.post('/api/:stream_key/:action/', 'tracker-update', actionView.asView);

};
