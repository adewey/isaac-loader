'use strict';
var View = require('../libs/views'),
    urls = require('../libs/urls'),
    tracker = require('../libs/tracker'),
    items = require('../libs/items'),
    trinkets = require('../libs/trinkets'),
    pockets = require('../libs/pockets'),
    sockets = require('../libs/sockets'),
    websockets = require('../libs/websockets');

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
        return res.render(trackerView.template, {user: req.session.user, data: trackerView.formatData(data)});
    });
};

websockets.on('connection', function (ws) {
    ws.on('message', function (rdata, flags) {
        if (flags.binary) { return; }
        console.log(rdata);
        var jsonObj = JSON.parse(rdata);
        var stream_key = jsonObj.stream_key;
        tracker.pickupItem(stream_key, jsonObj, function (err, data) {
            sockets.to(data.display_name).emit('update', trackerView.formatData(data));
        });
    });
    ws.on('close', function () {
        console.log('Connection closed!');
    });
    ws.on('error', function (e) {
        console.log(e)
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
    var charImgName = "playerportraitbig_01_isaac.png";
    if (data.characterid === 0) {
        charImgName = "playerportraitbig_01_isaac.png";
    } else if (data.characterid === 1) {
        charImgName = "playerportraitbig_02_magdalene.png";
    } else if (data.characterid === 2) {
        charImgName = "playerportraitbig_03_cain.png";
    } else if (data.characterid === 3) {
        charImgName = "playerportraitbig_04_judas.png";
    } else if (data.characterid === 4) {
        charImgName = "playerportraitbig_06_bluebaby.png";
    } else if (data.characterid === 5) {
        charImgName = "playerportraitbig_05_eve.png";
    } else if (data.characterid === 6) {
        charImgName = "playerportraitbig_07_samson.png";
    } else if (data.characterid === 7) {
        charImgName = "playerportraitbig_08_azazel.png";
    } else if (data.characterid === 8) {
        charImgName = "playerportraitbig_09_lazarus.png";
    } else if (data.characterid === 9) {
        charImgName = "playerportraitbig_09_eden.png";
    } else if (data.characterid === 10) {
        charImgName = "playerportraitbig_12_thelost.png";
    } else if (data.characterid === 11) {
        charImgName = "playerportraitbig_10_lazarus2.png";
    } else if (data.characterid === 12) {
        charImgName = "playerportraitbig_blackjudas.png";
    }

    return {
        display_name: data.display_name,
        items: items.list(data.items),
        trinkets: trinkets.list(data.trinkets),
        pockets: pockets.list(data.pockets),
        character: data.character,
        charImgName: charImgName,
        coins: data.coins,
        bombs: data.bombs,
        keys: data.keys,
        guppy: data.guppy,
        lof: data.lof,
        floor: data.floor,
        altfloor: data.altfloor,
        curses: currentCurses,
        charges: data.charges,
        speed: data.speed,
        range: data.range,
        shotspeed: data.shotspeed,
        tearrate: data.tearrate,
        damage: data.damage,
        luck: data.luck,
        seed: data.seed,
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
            tracker.pickupItem(stream_key, req.body, function(err, data) {
                sockets.to(data.display_name).emit('update', trackerView.formatData(data));
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