'use strict';

var parser = require('xml2json'),
    fs = require('fs');

var parsedFile = parser.toJson(fs.readFileSync('resources/items.xml', 'utf-8'), {object: true});

var json = {};

var passive = parsedFile.items.passive;
for (var i = 0, len = passive.length; i < len; i++)
{
    passive[i]['state'] = 'passive';
    if (passive[i]['gfx'])
        passive[i]['gfx'] = passive[i]['gfx'].toLowerCase()
    json[passive[i]['id']] = passive[i];
}

var active = parsedFile.items.active;
for (var i = 0, len = active.length; i < len; i++)
{
    active[i]['state'] = 'active';
    if (active[i]['gfx'])
        active[i]['gfx'] = active[i]['gfx'].toLowerCase()
    json[active[i].id] = active[i];
}
   
var familiar = parsedFile.items.familiar;
for (var i = 0, len = familiar.length; i < len; i++)
{
    familiar[i]['state'] = 'familiar';
    if (familiar[i]['gfx'])
        familiar[i]['gfx'] = familiar[i]['gfx'].toLowerCase()
    json[familiar[i].id] = familiar[i];
}


/*
var trinket = parsedFile.items.trinket;
for (var i in trinket)
{
    trinket[i]['state'] = 'trinket';
    json.trinket[trinket[i].id] = trinket[i];
}
*/

module.exports = json;