'use strict';

var challengeslist = require('../libs/resources').challenges;

module.exports.value = function(challenges) {
    return challengeslist[challenges];
};