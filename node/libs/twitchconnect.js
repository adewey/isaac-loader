var request = require('request');

//twitch oauth stuff
TwitchConnect = function(options) {

	this.client_id = options.client_id;
	this.secret = options.secret;
	this.uri = options.uri;
	this.scopes = options.scopes;
	this.url = 'https://api.twitch.tv/kraken';
	return this;
};

TwitchConnect.prototype.getRedirect = function(scopes) {
	if (!scopes)
		scopes = this.scopes;
	return 'https://api.twitch.tv/kraken/oauth2/authorize?response_type=token&client_id=' + this.client_id + '&redirect_uri=' + this.uri + '&scope=' + scopes.join(' ');;
};

//getUser:
//	oAuth = twitch supplied oAuth after log-in with twitch
TwitchConnect.prototype.getUser = function(oauth, callback) {
	var options = {
		url: this.url + '/user/',
		headers: {
			'Client-ID': this.client_id,
			'Accept': 'application/vnd.twitchtv.v3+json',
			'Authorization': 'OAuth ' + oauth,
		}
	};
	request(options, function(err, response, body) {
		if (err) return callback(err, response, body);
		body = JSON.parse(body);
		if (callback != undefined) callback(null, response, body);
	});
};

//getFollows:
//	user = twitch username
//	params = {
//		limit: 25			optional	integer	Maximum number of objects in array. Default is 25. Maximum is 100.
//		offset:	0			optional	integer	Object offset for pagination. Default is 0.
//		direction: desc		optional	string	Creation date sorting direction. Default is desc. Valid values are asc and desc.
//	}
TwitchConnect.prototype.getFollows = function(user, params, callback) {

	//construct our params to attach
	var p = '?';
	for(var key in params)
		p += key + '=' + params[key] + '&';
	var options = {
		url: this.url + '/channels/' + user + '/follows' + p.substring(0, p.length - 1),
		headers: {
			'Client-ID': this.client_id,
			'Accept': 'application/vnd.twitchtv.v3+json'
		}
	};
	request(options, function(err, response, body) {
		if (err) return callback(err, response, body);
		body = JSON.parse(body);
		if (callback != undefined) callback(null, response, body);
	});
};

//getSubscriptions:
//	oAuth = twitch supplied oAuth after log-in with twitch
//	user = twitch username
//	params = {
//		limit: 25			optional	integer	Maximum number of objects in array. Default is 25. Maximum is 100.
//		offset:	0			optional	integer	Object offset for pagination. Default is 0.
//		direction: desc		optional	string	Creation date sorting direction. Default is desc. Valid values are asc and desc.
//	}
TwitchConnect.prototype.getSubscriptions = function(oauth, user, params, callback) {

	//construct our params to attach
	var p = '?';
	for(var key in params)
		p += key + '=' + params[key] + '&';
	var options = {
		url: this.url + '/channels/' + user + '/subscriptions' + p.substring(0, p.length - 1),
		headers: {
			'Client-ID': this.client_id,
			'Accept': 'application/vnd.twitchtv.v3+json',
			'Authorization': 'OAuth ' + oauth,
		}
	};
	request(options, function(err, response, body) {
		if (err) return callback(err, response, body);
		body = JSON.parse(body);
		if (callback != undefined) callback(null, response, body);
	});
};

//getStream:
//	user = twitch username
TwitchConnect.prototype.getFollows = function(user, callback) {

	var options = {
		url: this.url + '/streams/' + user,
		headers: {
			'Client-ID': this.client_id,
			'Accept': 'application/vnd.twitchtv.v3+json'
		}
	};
	request(options, function(err, response, body) {
		if (err) return callback(err, response, body);
		body = JSON.parse(body);
		if (callback != undefined) callback(null, response, body);
	});
};

//getOAuth:
//	code = twitch provided auth code after log-in with twitch
TwitchConnect.prototype.getOAuth = function(code, callback) {

	var options = {
		url: this.url + '/oauth2/token/',
		form: { 
			'client_id': this.client_id,
			'client_secret': this.secret,
			'grant_type': 'authorization_code',
			'redirect_uri': this.uri,
			'code': code
		}
	};
	request.post(options, function(err, response, body) {
		if (err) return callback(err, response, body);
		body = JSON.parse(body);
		if (callback != undefined) callback(null, response, body);
	});
};

module.exports = TwitchConnect;