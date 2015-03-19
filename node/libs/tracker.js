'use strict';

var mongoose = require('mongoose'),
    Schema = mongoose.Schema,
    connection = mongoose.connection,
    uuid = require('node-uuid'),
    items = require('../libs/items');
    //_ = require('underscore');

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
    player_id: Number,
    challenge_id: Number,
    disable_achievements: Boolean,
    seed: String,
    floor: [Number], //0:NotStarted,1:StartedNormal,2:StartedAlt,3:ClearedNormal,4:ClearedAlt//
    altfloor: Number,
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
    range1: Number,
    range2: Number,
    shotspeed: Number,
    shotspeed1: Number,
    shotspeed2: Number,
    shotheight: Number,
    tearrate: Number,
    damage: Number,
    luck: Number,
    hard_mode: Boolean,
    version: Number,
    update_nag: Boolean,
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

/*
findUser('ditheren', function(err, user){
    user.admin = true;
    user.save();
});
findUser('yourruler', function(err, user){
    user.admin = true;
    user.save();
});
findUser('isaactracker', function(err, user){
    user.admin = true;
    user.save();
});
*/

function findUserByKey(stream_key, callback) {
    Tracker.findOne({stream_key: stream_key}, callback);
};


function formatUserData(user) {
    if (!user) return {};
    return user;
}


module.exports.regenerateUUID = function(display_name, callback) {
    findUser(display_name, function(err, user) {
        if (err || !user) return callback(err);
        user.stream_key = uuid.v4();
        user.save(function(err, res) {
            callback(err, formatUserData(res));
        });
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
        });
        console.log('\nNew User: ' + newUser.display_name);
        newUser.save(function(err, res) {
            callback(err, formatUserData(res));
        });
    });
};


module.exports.getUserData = function(display_name, callback) {
    findUser(display_name, function(err, user) {
        if (err || !user) return callback("unknown user: " + display_name);
        callback(err, formatUserData(user));
    });
};


module.exports.newGame = function(stream_key, data, callback) {
    findUserByKey(stream_key, function(err, user) {
        if (err || !user) return callback("unknown user: " + stream_key);
        user.updated_at = Date.now();
        user.markModified('updated_at');

        user.run_start = Date.now();
        user.markModified('run_start');
        
        if (data.seed !== undefined)
            user.seed = data.seed;
        
        if (data.curse !== undefined)
            user.curse = data.curse;
        
        if (data.hard_mode !== undefined)
            user.hard_mode = data.hard_mode;
            
        if (data.items !== undefined)
        {
            user.items = data.items;
            user.markModified('items');
        }
        
        if (data.itetrinketsms !== undefined)
        {
            user.trinkets = data.trinkets;
            user.markModified('trinkets');
        }
                 
        if (data.player_id !== undefined)
            user.player_id = data.player_id;

        if (data.characterid !== undefined)
            user.player_id = data.characterid;
            
        if (data.character_id !== undefined)
            user.player_id = data.character_id;
            
        if (data.challenge_id !== undefined)
            user.challenge_id = data.challenge_id;
            
        if (data.disable_achievements !== undefined)
            user.disable_achievements = data.disable_achievements;

        if (data.guppy !== undefined)
            user.guppy = data.guppy;
        
        if (data.lof !== undefined)
            user.lof = data.lof;
         
        if (data.charges !== undefined)
            user.charges = data.charges;
        
        if (data.speed !== undefined)
            user.speed = data.speed;
        
        if (data.range !== undefined)
            user.range = data.range;
        
        if (data.shotspeed !== undefined)
            user.shotspeed = data.shotspeed;
        
        if (data.shotheight !== undefined)
            user.shotheight = data.shotheight;
        
        if (data.shotspeed1 !== undefined)
            user.shotspeed1 = data.shotspeed1;
        
        if (data.shotspeed2 !== undefined)
            user.shotspeed2 = data.shotspeed2;
        
        if (data.range1 !== undefined)
            user.range1 = data.range1;
        
        if (data.range2 !== undefined)
            user.range2 = data.range2;
        
        if (data.tearrate !== undefined)
            user.tearrate = data.tearrate;
        
        if (data.damage !== undefined)
            user.damage = data.damage;
        
        if (data.luck !== undefined)
            user.luck = data.luck;
        
        if (data.version >= 1.2)
        {
            user.version = data.version
            user.items = [];
            user.markModified('items');
            user.seen_items = [];
            user.markModified('seen_items');
            user.floor = [];
            user.markModified('floor');
            user.trinkets = [];
            user.markModified('trinkets');
            user.keys = 0;
            user.bombs = 0;
            user.coins = 0;
            user.guppy = 0;
            user.lof = 0;
        } else {
            user.updatenag = true;
        }

        if (data.floor !== undefined)
        {
            user.floor = []
            user.floor[0] = data.floor + data.altfloor;
            user.markModified('floor');
        }
        
        if (data.altfloor !== undefined)
            user.altfloor = data.altfloor;
        

        user.killedbosses = []
        user.markModified('killedbosses');
        user.save(function(err, res) {
            callback(err, formatUserData(user));
        });
    });
};


module.exports.lostGame = function(stream_key, data, callback) {
    findUserByKey(stream_key, function(err, user) {
        if (err || !user) return callback("unknown user: " + stream_key);
        user.updated_at = Date.now();
        user.markModified('updated_at');
        user.run_end = Date.now();
        user.save(function(err, res) {
            callback(err, formatUserData(user));
        });
    });
};

module.exports.pickupItem = function(stream_key, data, callback) {
    findUserByKey(stream_key, function(err, user) {
        if (err || !user) return callback("unknown user: " + stream_key);
        user.updated_at = Date.now();
        user.markModified('updated_at');
        
        if (data.seed !== undefined)
            user.seed = data.seed;
        
        if (data.curse !== undefined)
            user.curse = data.curse;
        
        if (data.hard_mode !== undefined)
            user.hard_mode = data.hard_mode;
            
        if (data.items !== undefined)
        {
            user.items = data.items;
            user.markModified('items');
        }
        
        if (data.itetrinketsms !== undefined)
        {
            user.trinkets = data.trinkets;
            user.markModified('trinkets');
        }
         
        if (data.player_id !== undefined)
            user.player_id = data.player_id;

        if (data.characterid !== undefined)
            user.player_id = data.characterid;
            
        if (data.character_id !== undefined)
            user.player_id = data.character_id;

        if (data.guppy !== undefined)
            user.guppy = data.guppy;
        
        if (data.lof !== undefined)
            user.lof = data.lof;
         
        if (data.charges !== undefined)
            user.charges = data.charges;
        
        if (data.speed !== undefined)
            user.speed = data.speed;
        
        if (data.range !== undefined)
            user.range = data.range;
        
        if (data.shotspeed !== undefined)
            user.shotspeed = data.shotspeed;
        
        if (data.shotheight !== undefined)
            user.shotheight = data.shotheight;
        
        if (data.shotspeed1 !== undefined)
            user.shotspeed1 = data.shotspeed1;
        
        if (data.shotspeed2 !== undefined)
            user.shotspeed2 = data.shotspeed2;
        
        if (data.range1 !== undefined)
            user.range1 = data.range1;
        
        if (data.range2 !== undefined)
            user.range2 = data.range2;
        
        if (data.tearrate !== undefined)
            user.tearrate = data.tearrate;
        
        if (data.damage !== undefined)
            user.damage = data.damage;
        
        if (data.luck !== undefined)
            user.luck = data.luck;
        
        user.save(function(err, res) {
            callback(err, formatUserData(user));
        });
    });
};

module.exports.changeCharacter = function(stream_key, data, callback) {
    findUserByKey(stream_key, function(err, user) {
        if (err || !user) return callback("unknown user: " + stream_key);
        user.updated_at = Date.now();
        user.markModified('updated_at');
        user.player_id = data.player_id;
        user.save(function(err, res) {
            callback(err, formatUserData(user));
        });
    });
};


module.exports.updateFloor = function(stream_key, data, callback) {
    findUserByKey(stream_key, function(err, user) {
        if (err || !user) return callback("unknown user: " + stream_key);
        user.updated_at = Date.now();
        user.markModified('updated_at');
        
        if (data.seed !== undefined)
            user.seed = data.seed;
            
        if (data.floor == 11) data.floor = 10;
            
        if (user.floor[data.floor - 2])
            user.floor[data.floor - 2] += 2;
        user.floor[data.floor - 1] = data.altfloor + 1;
        user.markModified('floor');
        
        user.save(function(err, res) {
            callback(err, formatUserData(user));
        });
    });
};


module.exports.updateCurse = function(stream_key, data, callback) {
    findUserByKey(stream_key, function(err, user) {
        if (err || !user) return callback("unknown user: " + stream_key);
        user.updated_at = Date.now();
        user.markModified('updated_at');
        user.curse = data.curse;
        user.save(function(err, res) {
            callback(err, formatUserData(user));
        });
    });
};

module.exports.updateKeys = function (stream_key, data, callback) {
    findUserByKey(stream_key, function (err, user) {
        if (err || !user) return callback(err);
        user.updated_at = Date.now();
        user.markModified('updated_at');
        user.keys = data.keys;
        user.save(function (err, res) {
            callback(err, formatUserData(user));
        });
    });
};

module.exports.updateBombs = function (stream_key, data, callback) {
    findUserByKey(stream_key, function (err, user) {
        if (err || !user) return callback(err);
        user.updated_at = Date.now();
        user.markModified('updated_at');
        user.bombs = data.bombs;
        user.save(function (err, res) {
            callback(err, formatUserData(user));
        });
    });
};

module.exports.updateCoins = function (stream_key, data, callback) {
    findUserByKey(stream_key, function (err, user) {
        if (err || !user) return callback(err);
        user.updated_at = Date.now();
        user.markModified('updated_at');
        user.coins = data.coins;
        user.save(function (err, res) {
            callback(err, formatUserData(user));
        });
    });
};

module.exports.updateTrinkets = function (stream_key, data, callback) {
    findUserByKey(stream_key, function (err, user) {
        if (err || !user) return callback(err);
        user.updated_at = Date.now();
        user.markModified('updated_at');
        user.trinkets = data.trinkets;
        user.markModified('trinkets')
        user.save(function (err, res) {
            callback(err, formatUserData(user));
        });
    });
};

module.exports.updateItems = function (stream_key, data, callback) {
    findUserByKey(stream_key, function (err, user) {
        if (err || !user) return callback(err);
        user.updated_at = Date.now();
        user.markModified('updated_at');
        
        if (data.items !== undefined)
        {
            user.items = data.items;
            user.markModified('items');
        }
        
        if (data.trinkets !== undefined)
        {
            user.trinkets = data.trinkets;
            user.markModified('trinkets');
        }
        
        if (data.curse !== undefined)
            user.curse = data.curse;
        
        if (data.hard_mode !== undefined)
            user.hard_mode = data.hard_mode;
            
        if (data.items !== undefined)
        {
            user.items = data.items;
            user.markModified('items');
        }
        
        if (data.itetrinketsms !== undefined)
        {
            user.trinkets = data.trinkets;
            user.markModified('trinkets');
        }
                 
        if (data.player_id !== undefined)
            user.player_id = data.player_id;

        if (data.characterid !== undefined)
            user.player_id = data.characterid;
            
        if (data.character_id !== undefined)
            user.player_id = data.character_id;

        if (data.guppy !== undefined)
            user.guppy = data.guppy;
        
        if (data.lof !== undefined)
            user.lof = data.lof;
         
        if (data.charges !== undefined)
            user.charges = data.charges;
        
        if (data.speed !== undefined)
            user.speed = data.speed;
        
        if (data.range !== undefined)
            user.range = data.range;
        
        if (data.shotspeed !== undefined)
            user.shotspeed = data.shotspeed;
        
        if (data.shotheight !== undefined)
            user.shotheight = data.shotheight;
        
        if (data.shotspeed1 !== undefined)
            user.shotspeed1 = data.shotspeed1;
        
        if (data.shotspeed2 !== undefined)
            user.shotspeed2 = data.shotspeed2;
        
        if (data.range1 !== undefined)
            user.range1 = data.range1;
        
        if (data.range2 !== undefined)
            user.range2 = data.range2;
        
        if (data.tearrate !== undefined)
            user.tearrate = data.tearrate;
        
        if (data.damage !== undefined)
            user.damage = data.damage;
        
        if (data.luck !== undefined)
            user.luck = data.luck;
        
        if (data.seed !== undefined)
            user.seed = data.seed;

        user.save(function (err, res) {
            callback(err, formatUserData(user));
        });
    });
}
