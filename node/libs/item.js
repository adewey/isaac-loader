'use strict';

var resources = require('../libs/resources')

module.exports.list = function(items) {
    console.log(items);
    var list = [];
    for (var i = 0, len = items.length; i < len; i++)
        list.push(resources[items[i]]);
    console.log(list);
    return list;
};