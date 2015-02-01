'use strict';
/*
    TODO(Aaron):
    add base data that will be used across all views
 */
 
module.exports = function View(name) {
    this.name = name;
    /* fallback template name, incase it was not set */
    this.template = 'index';
};