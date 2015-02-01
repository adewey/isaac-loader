'use strict';
var View = require('../libs/views'),
    urls = require('../libs/urls'),
    twitch = require('../libs/twitch'),
    tracker = require('../libs/tracker');
/*
    twitch view
 */
    
var twitchView = new View('twitchView');
twitchView.asView = function(req, res) {
    var authenticated = (req.session.user !== undefined);
    switch(req.params.action)
    {
        case 'login':
            if (authenticated)
                return res.redirect(urls.reverse('index'));

            if (req.query.error) {
                console.log('error on login');
                return res.redirect(urls.reverse('index'));
            }

            //landing from twitch - we need to use javascript on the client side to redirect to get our access token
            if (!req.query.access_token && !req.session.refreshed)
            {
                req.session.refreshed = true;
                return res.render('twitch/index');
            }
            
            //store our access token/user in the session
            if (req.query.access_token)
            {
                twitch.getUser(req.query.access_token, function(err, request, body) {
                    if (err) return res.redirect('/twitch/login/?error=error');
                    delete req.session.refreshed;
                    req.session.user = body;
                    req.session.user.oauth = req.query.access_token;
                    tracker.getOrCreateUser(req.session.user.display_name, function(err, result) {
                        req.session.user.stream_key = result.stream_key;
                        return res.redirect('/');
                    });
                });
            }
            break;

        case 'logout':
            if (authenticated)
                delete req.session.user;
            return res.redirect(urls.reverse('index'));
            break;
            
        case 'redirect':
            if (!authenticated)
                return res.redirect(twitch.getRedirect());
            return res.redirect(urls.reverse('index'));
            break;

        // dont think we are actually going to render here, just redirect to the home page
        default:
            return res.redirect(urls.reverse('index'));
    }

};
 
module.exports.twitchView = twitchView;