'use strict';

var mongoose = require('mongoose'),
    Schema = mongoose.Schema,
    connection = mongoose.connection,
    uuid = require('node-uuid');

mongoose.connect('mongodb://localhost/trackerdb');

var trackerSchema = new Schema({
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
    seed: String,
    floor: String,
    curse: String,
    coins: Number,
    bombs: Number,
    keys: Number,
    items: [Number],
})
mongoose.model('Tracker', trackerSchema);
var Tracker = connection.model('Tracker', trackerSchema);


function findUser(display_name, callback) {
    Tracker.findOne({display_name: display_name}, callback);
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


module.exports.getOrCreateUser = function(display_name, callback) {
    findUser(display_name, function(err, res) {
        if (res) {
            return callback(err, formatUserData(res));
        }
        var newUser = new Tracker({display_name: display_name, stream_key: uuid.v4(), created_at: Date.now()});
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
            user.seed = data.seed;
            user.floor = data.floor;
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
            user.coins = data.coins;
            user.bombs = data.bombs;
            user.keys = data.keys;
            user.items = data.items;
            user.markModified('items');
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