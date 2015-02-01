'use strict';
var View = require('../libs/views'),
    urls = require('../libs/urls'),
    tracker = require('../libs/tracker'),
    items = require('../libs/items'),
    trinkets = require('../libs/trinkets'),
    pockets = require('../libs/pockets');

/*
    Home page view
 */
var indexView = new View('indexView');
indexView.template = 'index',
indexView.asView = function(req, res) {
    if ((req.query && req.query.reKey) && (req.session.user && req.session.user.display_name))
    {
        return tracker.regenerateUUID(req.session.user.display_name, function(err, result) {
            req.session.user.stream_key = result.stream_key;
            return res.render(indexView.template, {user: req.session.user});
        });
    }
    res.render(indexView.template, {user: req.session.user});
};

var trackerView = new View('trackerView');
trackerView.template = 'tracker/index',
trackerView.asView = function(req, res) {
    var display_name = req.params.display_name;
    tracker.getUserData(display_name, function(err, data) {
        if (!data || !data.display_name)
            return res.redirect(urls.reverse('index'));
        return res.render(trackerView.template, trackerView.formatData(data));
    });
};

trackerView.formatData = function(data) {
    return {
        display_name: data.display_name,
        items: items.list(data.items),
        trinkets: trinkets.list(data.trinkets),
        pockets: pockets.list(data.pockets),
        coins: data.coins,
        bombs: data.bombs,
        keys: data.keys,
        updated_at: data.updated_at,
    };
};
    
var actionView = new View('actionView');
actionView.asView = function(req, res) {
    var action = req.params.action,
        stream_key = req.params.stream_key;

    switch(action) {
        case 'new':
            //params: seed
            tracker.newGame(stream_key, req.body, function() {
                res.status(200).send();
            });
            return;
        case 'lost':
            tracker.lostGame(stream_key, req.body, function() {
                res.status(200).send();
            });
            return;
        case 'seen':
            //possibly will not be used
            tracker.seenItem(stream_key, req.body, function() {
                res.status(200).send();
            });
            return;
        case 'pickup':
            //this is the only one we use currently
            console.log(req.body);
            tracker.pickupItem(stream_key, req.body, function() {
                res.status(200).send();
            });
            return;
        case 'character':
            tracker.changeCharacter(stream_key, req.body, function() {
                res.status(200).send();
            });
            return;
        case 'floor':
            tracker.updateFloor(stream_key, req.body, function() {
                res.status(200).send();
            });
            return;
        case 'curse':
            tracker.updateCurse(stream_key, req.body, function() {
                res.status(200).send();
            });
            return;
        default:
            break;
    };

    // dont think we are actually going to render here, just redirect to the user page if it is non ajax calls
    return res.redirect(urls.reverse('tracker-detail', {display_name: display_name}));
};
 
module.exports = {
    'indexView': indexView,
    'trackerView': trackerView,
    'actionView': actionView,
};