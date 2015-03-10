'use strict';

var mongoose = require('mongoose'),
    Schema = mongoose.Schema,
    connection = mongoose.connection,
    uuid = require('node-uuid'),
    items = require('../libs/items'),
    _ = require('underscore');

mongoose.connect('mongodb://localhost/trackerdb');

var trackerSchema = new Schema({
    admin: {
        type: Boolean,
        default: false,
    },
    display_name: {
        type: String,
        unique: true,
    },
    created_at: {
        type: Date,
    },
    updated_at: {
        type: Date,
    },
    stream_key: {
        type: String,
        unique: true,
    },
    run_start: Date,
    run_end: Date,
    character: String,
    characterid: Number,
    seed: String,
    floor: String,
    altfloor: String,
    curse: String,
    coins: Number,
    bombs: Number,
    charges: Number,
    keys: Number,
    trinkets: [Number],
    pockets: [Schema.Types.Mixed],
    items: [Number],
    seen_items: [Number],
    guppy: Number,
    lof: Number,
    speed: Number,
    range: Number,
    shotspeed: Number,
    tearrate: Number,
    damage: Number,
    luck: Number,
    killedbosses: [Number] //0 means not encountered/killed,1+ == boss entity ID
})
mongoose.model('Tracker', trackerSchema);
var Tracker = connection.model('Tracker', trackerSchema);

function findUsers(callback) {
    Tracker.find({}).sort('-updated_at').exec(callback);
};

function findUser(display_name, callback) {
    Tracker.findOne({display_name: { $regex : new RegExp(display_name, "i") }}, callback);
};

function findUserByKey(stream_key, callback) {
    Tracker.findOne({stream_key: stream_key}, callback);
};


function formatUserData(user) {
    if (!user) return {};
    return user;
}


module.exports.regenerateUUID = function(display_name, callback) {
    findUser(display_name, function(err, res) {
        if (err) return callback(err);
        var user = undefined;
        if (res) {
            user = res;
            user.stream_key = uuid.v4();
            user.save(function(err, res) {
                return callback(err, formatUserData(res));
            });
            return;
        }
        return callback(err, formatUserData(user));
    });
};

module.exports.getUsers = function (callback) {
    findUsers(function (err, res) {
        if (err) return callback(err);
        if (res) {
            return callback(err, res);
        }
    });
}
module.exports.getOrCreateUser = function(display_name, callback) {
    findUser(display_name, function(err, res) {
        if (err) return callback(err);
        if (res) {
            return callback(err, formatUserData(res));
        }
        var newUser = new Tracker({
            display_name: display_name,
            stream_key: uuid.v4(),
            created_at: Date.now(),
            updated_at: Date.now(),
            run_start: Date.now(),
            seed: "",
            floor: [Number], //0:NotStarted,1:StartedNormal,2:StartedAlt,3:ClearedNormal,4:ClearedAlt//
            altfloor: 0,
            curse: 0,
            coins: 0,
            bombs: 0,
            charges: 0,
            keys: 0,
            guppy: 0,
            lof: 0,
            speed: 1,
            range: 1,
            shotspeed: 1,
            tearrate: 1,
            damage: 1,
            luck: 1,
        });
        console.log('\nNew User: ' + newUser.display_name);
        newUser.save(function(err, res) {
            return callback(err, formatUserData(res));
        });
    });
};


module.exports.getUserData = function(display_name, callback) {
    findUser(display_name, function(err, res) {
        if (err || !res) return callback("unknown user: " + stream_key);
        var user = undefined;
        if (res)
            user = res;
        return callback(err, formatUserData(user));
    });
};


module.exports.newGame = function(stream_key, data, callback) {
    findUserByKey(stream_key, function(err, res) {
        if (err || !res) return callback("unknown user: " + stream_key);
        var user = undefined;
        if (res) {
            user = res;
            user.updated_at = Date.now();
            user.run_start = Date.now();
            user.run_end = null;
            user.character = data.character;
            user.characterid = data.characterid;
            user.seed = data.seed;
            for (var i = 0; i < 10; i++) {
                user.floor[i] = 0;
            }
            if (data.altfloor) {
                user.floor[data.floor] = 1;
            } else {
                user.floor[data.floor] = 2;
            }
            user.curse = data.curse;
            user.items = data.items;
            user.seen_items = data.items;
            user.damage = data.damage;
            user.speed = data.speed;
            user.shotspeed = data.shotspeed;
            user.tearrate = data.tearrate;
            user.range = data.range;
            user.luck = data.luck;
            for (var i = 0; i < 10; i++) {
                user.killedbosses[i] = 0;
            }
            return user.save(function(err, res) {
                return callback(err, formatUserData(user));
            });
        }
        return callback(err, formatUserData(user));
    });
};


module.exports.lostGame = function(stream_key, data, callback) {
    findUserByKey(stream_key, function(err, res) {
        if (err || !res) return callback("unknown user: " + stream_key);
        var user = undefined;
        if (res) {
            user = res;
            user.updated_at = Date.now();
            user.run_end = Date.now();
            return user.save(function(err, res) {
                return callback(err, formatUserData(user));
            });
        }
        return callback(err, formatUserData(user));
    });
};


//module.exports.seenItem = function(stream_key, data, callback) {
//    findUserByKey(stream_key, function(err, res) {
//        if (err || !res) return callback(err);
//        var user = undefined;
//        if (res) {
//            user = res;
//            user.updated_at = Date.now();
//            user.seen_items.append(data.item);
//            user.markModified('seen_items');
//            return user.save(function(err, res) {
//                return callback(err, formatUserData(user));
//            });
//        }
//        return callback(err, formatUserData(user));
//    });
//};


module.exports.pickupItem = function(stream_key, data, callback) {
    findUserByKey(stream_key, function(err, res) {
        if (err || !res) return callback("unknown user: " + stream_key);
        var user = undefined;
        if (res) {
            user = res;
            user.updated_at = Date.now();
            user.character = data.character;
            user.characterid = data.characterid;
            user.coins = data.coins;
            user.bombs = data.bombs;
            user.keys = data.keys;
            user.items = data.items;
            user.markModified('items');
            user.trinkets = data.trinkets;
            user.markModified('trinkets');
            user.pockets = data.pockets;
            user.markModified('pockets');
            user.guppy = data.guppy;
            user.lof = data.lof;
            user.floor = data.floor;
            user.altfloor = data.altfloor;
            user.curse = data.curses;
            user.charges = data.charges;
            user.speed = data.speed;
            user.range = data.range;
            user.shotspeed = data.shotspeed;
            user.tearrate = data.tearrate;
            user.damage = data.damage;
            user.luck = data.luck;
            user.seed = data.seed;
            return user.save(function(err, res) {
                return callback(err, formatUserData(user));
            });
        }
        return callback(err, formatUserData(user));
    });
};


module.exports.changeCharacter = function(stream_key, data, callback) {
    findUserByKey(stream_key, function(err, res) {
        if (err || !res) return callback("unknown user: " + stream_key);
        var user = undefined;
        if (res) {
            user = res;
            user.updated_at = Date.now();
            user.character = data.character;
            return user.save(function(err, res) {
                return callback(err, formatUserData(user));
            });
        }
        return callback(err, formatUserData(user));
    });
};


module.exports.updateFloor = function(stream_key, data, callback) {
    findUserByKey(stream_key, function(err, res) {
        if (err || !res) return callback("unknown user: " + stream_key);
        var user = undefined;
        if (res) {
            user = res;
            user.updated_at = Date.now();
            user.floor = data.floor;
            return user.save(function(err, res) {
                return callback(err, formatUserData(user));
            });
        }
        return callback(err, formatUserData(user));
    });
};


module.exports.updateCurse = function(stream_key, data, callback) {
    findUserByKey(stream_key, function(err, res) {
        if (err || !res) return callback("unknown user: " + stream_key);
        var user = undefined;
        if (res) {
            user = res;
            user.updated_at = Date.now();
            user.curse = data.curse;
            return user.save(function(err, res) {
                return callback(err, formatUserData(user));
            });
        }
        return callback(err, formatUserData(user));
    });
};

module.exports.updateKeys = function (stream_key, data, callback) {
    findUserByKey(stream_key, function (err, res) {
        if (err || !res) return callback(err);
        var user = undefined;
        if (res) {
            user = res;
            user.updated_at = Date.now();
            user.keys = data.keys;
            return user.save(function (err, res) {
                return callback(err, formatUserData(user));
            });
        }
    });
};

module.exports.updateBombs = function (stream_key, data, callback) {
    findUserByKey(stream_key, function (err, res) {
        if (err || !res) return callback(err);
        var user = undefined;
        if (res) {
            user = res;
            user.updated_at = Date.now();
            user.bombs = data.bombs;
            return user.save(function (err, res) {
                return callback(err, formatUserData(user));
            });
        }
    });
};

module.exports.updateItems = function (stream_key, data, callback) {
    findUserByKey(stream_key, function (err, res) {
        if (err || !res) return callback(err);
        var user = undefined;
        if (res) {
            user = res;
            user.updated_at = Date.now();
            user.items = data.items;
            user.seen_items = _.union(user.seen_items, user.items);
            for (var i; i < user.seen_items.length; i++){
                console.log("seenItems" + user.seen_items[i]);
            }
            user.damage = data.damage;
            user.speed = data.speed;
            user.shotspeed = data.shotspeed;
            user.tearrate = data.tearrate;
            user.range = data.range;
            user.luck = data.luck;
            return user.save(function (err, res) {
                return callback(err, formatUserData(user));
            });
        }
    });
}
