'use strict';

var playerslist = require('../libs/resources').players;

module.exports.value = function(players) {
    return playerslist[players];
};