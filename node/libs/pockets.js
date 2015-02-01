'use strict';

var cardlist = require('../libs/resources').cardlist,
    pilllist = require('../libs/resources').pills;

module.exports.list = function(pockets) {
    var list = [];
    for (var i = 0, len = pockets.length; i < len; i++)
    {
        if (pockets.is_card)
            list.push(cardlist[pockets[i]]);
        else
            list.push(pilllist[pockets[i]]);
    }
    return list;
};