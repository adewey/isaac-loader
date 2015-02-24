'use strict';
var View = require('../libs/views'),
    urls = require('../libs/urls'),
    tracker = require('../libs/tracker'),
    items = require('../libs/items'),
    trinkets = require('../libs/trinkets'),
    pockets = require('../libs/pockets'),
    sockets = require('../libs/sockets'),
    websockets = require('../libs/websockets'),
    zipper = require('../libs/zipper');

/*
    Home page view
 */
var adminView = new View('adminView');
adminView.template = 'admin',
adminView.asView = function (req, res) {
    if (!req.session.user) {
        return res.redirect(urls.reverse('index'));
    } else {
        if (!req.session.user.admin) {
            return res.redirect(urls.reverse('index'));
        }
    }
    tracker.getUsers(function (err,data) {
        if (!data)
            return res.redirect(urls.reverse('index'));
        res.render(adminView.template, { user: req.session.user, users: data });
    });
};

module.exports = {
    'adminView': adminView,
};