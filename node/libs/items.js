'use strict';

var itemlist = require('../libs/resources').items;

module.exports.list = function(items) {
    var list = [];
    for (var i = 0, len = items.length; i < len; i++)
        list.push(itemlist[items[i]]);
    return list;
};