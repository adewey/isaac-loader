'use strict';
/*
    TODO(Aaron):
    namespacing
        or parent relationships
 */


/* urls: an array of class Url */
var urls = [];

var Route = function (url, name) {
    this.url = url;
    this.name = name;
}

var Url = function (url, router, baseurl) {
    if (baseurl && baseurl.url)
    {
        console.log(baseurl.url);
        url = baseurl.url.replace(/\/^/, '') + url;
    }
    this.url = url;
    this.router = router;
    this.routes = [];
    this.addRoute = function (url, name) {
        var route = new Route(url, name);
        this.routes.push(route);
        return route;
    };
    this.get = function (url, name, callback) {
        this.router.get(url, callback);
        return this.addRoute(url, name);
    };
    this.delete = function (url, name, callback) {
        this.router.delete(url, callback);
        return this.addRoute(url, name);
    };
    this.post = function (url, name, callback) {
        this.router.post(url, callback);
        return this.addRoute(url, name);
    };
    this.put = function (url, name, callback) {
        this.router.put(url, callback);
        return this.addRoute(url, name);
    };
    this.patch = function (url, name, callback) {
        this.router.patch(url, callback);
        return this.addRoute(url, name);
    };
    this.all = function (url, name, callback) {
        this.router.all(url, callback);
        return this.addRoute(url, name);
    };
    
    this.findRouteByName = function (name) {
        for (var index in this.routes) {
            var route = this.routes[index];
            if (route && route.name && route.name == name)
                return route;
        }
    }
    
};

Url.add = function (url, router) {
    var url = new Url(url, router);
    urls.push(url);
    return url;
};
    
Url.findByName = function (name) {
    for (var index in this.urls) {
        var url = urls[index];
        if (url && url.name && url.name == name)
            return url;
    }
};
    
Url.findByRoute = function (route) {
    for (var index in this.urls) {
        var url = urls[index];
        //todo: implement this
    }
};
    
Url.reverse = function (name) {
    for (var index in this.urls) {
        var url = urls[index];
        if (url && url.name && url.name == name)
            return url.url;
    }
    // if we can't find the route, just return us to home..
    return '/';
};


module.exports = Url;