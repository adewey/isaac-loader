'use strict';

var parser = require('xml2json'),
    fs = require('fs');

var itemFile = parser.toJson(fs.readFileSync('resources/items.xml', 'utf-8'), {object: true});
var pocketFile = parser.toJson(fs.readFileSync('resources/pocketitems.xml', 'utf-8'), {object: true});

var json = {items:{},trinkets:{},cards:{},pills:{}};

var passive = itemFile.items.passive;
for (var i = 0; i < passive.length; i++)
{
    passive[i]['state'] = 'passive';
    if (passive[i]['gfx'])
        passive[i]['gfx'] = passive[i]['gfx'].toLowerCase()
    json.items[passive[i]['id']] = passive[i];
}

var active = itemFile.items.active;
for (var i = 0; i < active.length; i++)
{
    active[i]['state'] = 'active';
    if (active[i]['gfx'])
        active[i]['gfx'] = active[i]['gfx'].toLowerCase()
    json.items[active[i].id] = active[i];
}
   
var familiar = itemFile.items.familiar;
for (var i = 0; i < familiar.length; i++)
{
    familiar[i]['state'] = 'familiar';
    if (familiar[i]['gfx'])
        familiar[i]['gfx'] = familiar[i]['gfx'].toLowerCase()
    json.items[familiar[i].id] = familiar[i];
}

var trinket = itemFile.items.trinket;
for (var i = 0; i < trinket.length; i++)
{
    json.trinkets[trinket[i].id] = trinket[i];
}

var card = pocketFile.pocketitems.card;
for (var i = 0; i < card.length; i++)
{
    json.cards[card[i].id] = card[i];
}

var rune = pocketFile.pocketitems.rune;
for (var i = 0; i < rune.length; i++)
{
    json.cards[rune[i].id] = rune[i];
}

var pilleffect = pocketFile.pocketitems.pilleffect;
for (var i = 0; i < pilleffect.length; i++)
{
    json.pills[pilleffect[i].id] = pilleffect[i];
}


module.exports = json;