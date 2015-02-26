'use strict';

var mongoose = require('mongoose'),
    Schema = mongoose.Schema,
    connection = mongoose.connection,
    uuid = require('node-uuid');

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
    guppy: Number,
    lof: Number,
    speed: Number,
    range: Number,
    shotspeed: Number,
    tearrate: Number,
    damage: Number,
    luck: Number,
})
mongoose.model('Tracker', trackerSchema);
var Tracker = connection.model('Tracker', trackerSchema);

function findUsers(callback) {
    Tracker.find({}, callback);
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
        if (res) {
            return callback(err, res);
        }
    });
}
module.exports.getOrCreateUser = function(display_name, callback) {
    findUser(display_name, function(err, res) {
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
            floor: 1,
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
        newUser.save(function(err, res) {
            return callback(err, formatUserData(res));
        });
    });
};


module.exports.getUserData = function(display_name, callback) {
    findUser(display_name, function(err, res) {
        var user = undefined;
        if (res)
            user = res;
        return callback(err, formatUserData(user));
    });
};


module.exports.newGame = function(stream_key, data, callback) {
    findUserByKey(stream_key, function(err, res) {
        var user = undefined;
        if (res) {
            user = res;
            user.updated_at = Date.now();
            user.run_start = Date.now();
            user.run_end = null;
            user.character = data.character;
            user.characterid = data.characterid;
            user.seed = data.seed;
            user.floor = data.floor;
            user.altfloor = data.altfloor;
            user.curse = data.curse;
            user.items = data.items;
            user.seen_items = data.items;
            return user.save(function(err, res) {
                return callback(err, formatUserData(user));
            });
        }
        return callback(err, formatUserData(user));
    });
};


module.exports.lostGame = function(stream_key, data, callback) {
    findUserByKey(stream_key, function(err, res) {
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


module.exports.seenItem = function(stream_key, data, callback) {
    findUserByKey(stream_key, function(err, res) {
        var user = undefined;
        if (res) {
            user = res;
            user.updated_at = Date.now();
            user.seen_items.append(data.item);
            user.markModified('seen_items');
            return user.save(function(err, res) {
                return callback(err, formatUserData(user));
            });
        }
        return callback(err, formatUserData(user));
    });
};


module.exports.pickupItem = function(stream_key, data, callback) {
    findUserByKey(stream_key, function(err, res) {
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