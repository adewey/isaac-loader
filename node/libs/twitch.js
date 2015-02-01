'use strict';

var twitch = require('./twitchconnect');

module.exports = new twitch({
	client_id: 'tr2qxqz4wma7etfqkesrvv63nkeykao',
	secret: 'j3229d1cgbwrdcyq8ddlzpn1f8tdoct',
	uri: 'http://www.isaactracker.com/twitch/login/',
	scopes: ['user_read']
});
