'use strict';

var trinketlist = require('../libs/resources').trinkets;

module.exports.list = function(trinkets) {
    var list = [];
    for (var i = 0, len = trinkets.length; i < len; i++)
        if (trinkets[i])
            list.push(trinketlist[trinkets[i]]);
    return list;
};