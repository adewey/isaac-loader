'use strict';
var View = require('../libs/views'),
    urls = require('../libs/urls'),
    tracker = require('../libs/tracker'),
    items = require('../libs/items'),
    trinkets = require('../libs/trinkets'),
    pockets = require('../libs/pockets'),
    players = require('../libs/players'),
    challenges = require('../libs/challenges'),
    sockets = require('../libs/sockets'),
    websockets = require('../libs/websockets'),
    //_ = require('underscore'),
    zipper = require('../libs/zipper');

/*
    Home page view
 */
var indexView = new View('indexView');
indexView.template = 'index',
indexView.asView = function(req, res) {
    /* this shouldn't happen, we don't tell people they can regenerate their keys anymore, and we don't display them on the website */
    if ((req.query && req.query.reKey) && (req.session.user && req.session.user.display_name))
    {
        return tracker.regenerateUUID(req.session.user.display_name, function(err, result) {
            req.session.user.stream_key = result.stream_key;
            return res.render(indexView.template, {user: req.session.user});
        });
    }
    res.render(indexView.template, {user: req.session.user});
};

var faqView = new View('faqView');
faqView.template = 'faq',
faqView.asView = function(req, res) {
    res.render(faqView.template, {user: req.session.user});
};

var privacyView = new View('privacyView');
privacyView.template = 'privacy',
privacyView.asView = function(req, res) {
    res.render(privacyView.template, {user: req.session.user});
};

var aboutView = new View('aboutView');
aboutView.template = 'about',
aboutView.asView = function (req, res) {
    res.render(aboutView.template, { user: req.session.user });
};

var downloadView = new View('downloadView');
downloadView.asView = function(req, res) {
    if (!req.session.user || !req.session.user.stream_key)
        return res.redirect(urls.reverse('index'));
    return zipper(res, req.session.user.stream_key);
};

var trackerView = new View('trackerView');
trackerView.template = 'tracker/index',
trackerView.asView = function(req, res) {
    var display_name = req.params.display_name;
    tracker.getUserData(display_name, function(err, data) {
        if (!data || !data.display_name)
            return res.redirect(urls.reverse('index'));
        return res.render(trackerView.template, {user: req.session.user, data: trackerView.formatData(data)});
    });
};

websockets.on('connection', function (ws) {
    var user;
    if (ws.upgradeReq.headers.origin != "Client")
        user = ws.upgradeReq.headers.origin;
    ws.on('message', function (rdata, flags) {
        if (flags.binary) { return; }
        if (ws.upgradeReq.headers.origin == "Client") {
            var jsonObj = JSON.parse(rdata);
            var stream_key = jsonObj.stream_key;
            if (user == null) {
                user = stream_key;
                console.log("Connected: " + Date(), user);
            }
            console.log("Message: " + rdata, user);
            tracker.pickupItem(stream_key, jsonObj, function (err, data) {
                if (err || !data) return console.log('Error: ' + err, user);
                sockets.to(data.display_name).emit('update', trackerView.formatData(data));
            });
        } else { //So this is the new code, that no one will hit except Brett right now :D
            //console.log(rdata);
            var updateData = JSON.parse(rdata);
            if (updateData) {
                if (updateData.action == "updateKeys") {
                    tracker.updateKeys(user, updateData, function (err, data) {
                        if (err || !data) return console.log('Error: ' + err, user);
                        sockets.to(data.display_name).emit('update', trackerView.formatData(data));
                    });
                } else if (updateData.action == "updateBombs") {
                    tracker.updateBombs(user, updateData, function (err, data) {
                        if (err || !data) return console.log('Error: ' + err, user);
                        sockets.to(data.display_name).emit('update', trackerView.formatData(data));
                    });
                } else if (updateData.action == "updateFloor") {
                    tracker.updateFloor(user, updateData, function (err, data) {
                        if (err || !data) return console.log('Error: ' + err, user);
                        sockets.to(data.display_name).emit('update', trackerView.formatData(data));
                    });
                } else if (updateData.action == "updateCoins") {
                    tracker.updateCoins(user, updateData, function (err, data) {
                        if (err || !data) return console.log('Error: ' + err, user);
                        sockets.to(data.display_name).emit('update', trackerView.formatData(data));
                    });
                } else if (updateData.action == "updateItems") {
                    tracker.updateItems(user, updateData, function (err, data) {
                        if (err || !data) return console.log('ERROR: ' + err, user);
                        sockets.to(data.display_name).emit('update', trackerView.formatData(data));
                    });
                } else if (updateData.action == "updateTrinkets") {
                    tracker.updateTrinkets(user, updateData, function (err, data) {
                        if (err || !data) return console.log('ERROR: ' + err, user);
                        sockets.to(data.display_name).emit('update', trackerView.formatData(data));
                    });
                } else if (updateData.action == "newGame") {
                    tracker.newGame(user, updateData, function (err, data) {
                        if (err || !data) return console.log('ERROR: ' + err, user);
                        sockets.to(data.display_name).emit('update', trackerView.formatData(data));
                        /*
                        setTimeout(function() {
                            ws.send(JSON.stringify({action: "fortune",
                                                    line1: "",
                                                    line2: "isaactracker loaded!",
                                                    line3: ""
                                                  })
                            );
                        }, 5000);
                        */
                    });
                }
            }
        }
    });
    ws.on('close', function close() {
        if (user != null) {
            console.log("Disconnected: " + Date(), user);
        }
    });
    ws.on('error', function (e) {
        console.log(e, user)
        console.log(e);
    });
});

trackerView.formatData = function (data) {
    var curseValsJSON = { "-64": "Curse of The Blind!", "-32": "Curse of the Maze!", "-16": "Curse of the Cursed!", "-8": "Curse of The Unknown!", "-4": "Curse of The Lost!", "-2": "Curse of the Labyrinth!", "-1": "Curse of Darkness!" }
    var currentCurseNum = data.curse;
    var currentCurses = {};
    for (var key in curseValsJSON) {
        if (curseValsJSON.hasOwnProperty(key)) {
            if (currentCurseNum >= -(key)) {
                currentCurseNum -= -(key);
                currentCurses[-(key)] = curseValsJSON[key];
            }
        }
    }
    
    if (data.guppy > 3) data.guppy = 3;
    
    if (data.lof > 3) data.lof = 3;  
    
    data.speed = Math.floor(data.speed + data.speed)
    if (data.speed < 1) data.speed = 1;
    if (data.speed > 7) data.speed = 7;
    
    data.tearrate = Math.floor(15.0 / data.tearrate)
    if (data.tearrate < 1) data.tearrate = 1;
    if (data.tearrate > 7) data.tearrate = 7;
    
    data.damage = Math.floor(data.damage / 1.5)
    if (data.damage < 1) data.damage = 1;
    if (data.damage > 7) data.damage = 7;
    
    if (data.shotheight !== undefined) {
        data.range = Math.floor((data.shotspeed - 1.0) / 0.15 + 1.0 + data.shotheight + data.shotheight + (-data.range / 20.0))
    }
    else {
        data.range = data.range + data.range;
    }
    if (data.range < 1) data.range = 1;
    if (data.range > 7) data.range = 7;

    data.shotspeed = Math.floor(data.shotspeed + data.shotspeed)
    if (data.shotspeed < 1) data.shotspeed = 1;
    if (data.shotspeed > 7) data.shotspeed = 7; 
    
    if (data.luck < 1) data.luck = 1;
    if (data.luck > 7) data.luck = 7;
    
    
    //number of people in this room
    try {
	var s = sockets.nsps['/'].adapter.rooms[data.display_name];
	var o = Object.keys(s).length;
        data.room_count = o; 
    } catch (err) {
        //console.log('Error getting room count');
    }
    /*
    data.seen_items = _.uniq(data.seen_items);
    for (var i = 0; i < data.seen_items.length; i++) {
        console.log("Seen Item: " + data.seen_items[i]);
    }
    for (var i = 0; i < data.items.length; i++) {
        console.log("Seen Item: " + data.items[i]);
    }
    data.seen_items = _.difference((data.seen_items), (data.items));
    */
    return {
        bombs: data.bombs || 0,
        challenge: challenges.value(data.challenge_id || 0),
        charges: data.charges,
        coins: data.coins || 0,
        curses: currentCurses,
        damage: data.damage,
        disable_achievements: data.disable_achievements || false,
        display_name: data.display_name,
        floor: data.floor || [],
        guppy: data.guppy || 0,
        items: items.list(data.items),
        hard_mode: data.hard_mode || false,
        keys: data.keys || 0,
        lof: data.lof || 0,
        luck: data.luck,
        player: players.value(data.player_id || 0),
        pockets: pockets.list(data.pockets),
        range: data.range,
        room_count: data.room_count || 0,
        seed: data.seed,
        seen_items: items.list(data.seen_items),
        shotspeed: data.shotspeed,
        speed: data.speed,
        tearrate: data.tearrate,
        trinkets: trinkets.list(data.trinkets),
        updated_at: data.updated_at,
    };
};
 
module.exports = {
    'indexView': indexView,
    'faqView': faqView,
    'privacyView': privacyView,
    'aboutView': aboutView,
    'downloadView': downloadView,
    'trackerView': trackerView,
};
