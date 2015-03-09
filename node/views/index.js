'use strict';
var View = require('../libs/views'),
    urls = require('../libs/urls'),
    tracker = require('../libs/tracker'),
    items = require('../libs/items'),
    trinkets = require('../libs/trinkets'),
    pockets = require('../libs/pockets'),
    sockets = require('../libs/sockets'),
    websockets = require('../libs/websockets'),
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
    var user = ws.upgradeReq.headers.origin;
    ws.send("{ \"action\" : \"fortune\" , \"line1\" : \"\" , \"line2\" : \"Isaac Tracker Connected!\" , \"line3\" : \"\" }");
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
                sockets.to(data.display_name).emit('update', trackerView.formatData(data));
            });
        } else { //So this is the new code, that no one will hit except Brett right now :D
            console.log(rdata);
            var updateData = JSON.parse(rdata);
            if (updateData) {
                if (updateData.action == "updateKeys") {
                    tracker.updateKeys(user, updateData, function (err, data) {
                        sockets.to(data.display_name).emit('update', trackerView.formatData(data));
                    });
                } else if (updateData.action == "updateBombs") {
                    tracker.updateBombs(user, updateData, function (err, data) {
                        sockets.to(data.display_name).emit('update', trackerView.formatData(data));
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
    if (data.guppy > 3) {
        data.guppy = 3;
    }
    if (data.lof > 3) {
        data.lof = 3;
    }
    if (data.floor == 11) data.floor = 10;
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
    var barsUsed = 0;
    if (data.tearrate > 7){
        barsUsed = 1;
    }else if(data.tearrate > 5){
        barsUsed = 2;
    }else if(data.tearrate > 3){
        barsUsed = 4;
    }else{
        barsUsed = 7;
    }
    data.tearrate = barsUsed;
    
    if (data.damage < 3){
        barsUsed = 1;
    }else if(data.damage< 4.5){
        barsUsed = 2;
    }else if(data.damage < 6.0){
        barsUsed = 3;
    }else if(data.damage < 7.5){
        barsUsed = 4;
    }else if(data.damage < 9){
        barsUsed = 5;
    }else if(data.damage < 10.5){
        barsUsed = 6;
    }else{
        barsUsed = 7;
    }
    data.damage = barsUsed;

    if (data.range < 0){
        barsUsed = 1;
    }else if(data.range < .5){
        barsUsed = 2;
    }else if(data.range < 1.0){
        barsUsed = 3;
    }else if(data.range < 1.5){
        barsUsed = 4;
    }else if(data.range < 2){
        barsUsed = 5;
    }else if(data.range < 2.5){
        barsUsed = 6;
    }else{
        barsUsed = 7;
    }
    data.range = barsUsed;
    
    if (data.shotspeed < 1){
        barsUsed = 1;
    }else if(data.shotspeed < 1.2){
        barsUsed = 2;
    }else if(data.shotspeed < 1.0){
        barsUsed = 3;
    }else if(data.shotspeed < 1.3){
        barsUsed = 4;
    }else if(data.shotspeed < 1.6){
        barsUsed = 5;
    }else if(data.shotspeed < 1.8){
        barsUsed = 6;
    }else{
        barsUsed = 7;
    }
    data.shotspeed = barsUsed;
    
    if (data.speed < 1){
        barsUsed = 1;
    }else if(data.speed < 1.5){
        barsUsed = 2;
    }else if(data.speed < 2){
        barsUsed = 3;
    }else if(data.speed < 2.5){
        barsUsed = 4;
    }else if(data.speed < 3){
        barsUsed = 5;
    }else if(data.speed < 3.5){
        barsUsed = 6;
    }else{
        barsUsed = 7;
    }
    data.speed = barsUsed;
    
    if (data.luck < 2){
        barsUsed = 1;
    }else if(data.luck < 3){
        barsUsed = 2;
    }else if(data.luck < 4){
        barsUsed = 3;
    }else if(data.luck < 5){
        barsUsed = 4;
    }else if(data.luck < 6){
        barsUsed = 5;
    }else if(data.luck < 7){
        barsUsed = 6;
    }else{
        barsUsed = 7;
    }
    data.luck = barsUsed;
    
    //number of people in this room
    try {
	var s = sockets.nsps['/'].adapter.rooms[data.display_name];
	var o = Object.keys(s).length;
        data.room_count = o; 
    } catch (err) {
        //console.log('Error getting room count');
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
        room_count: data.room_count,
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
