'use strict';

var archiver = require('archiver');

module.exports = function(res, key) {
    res.set({
        'Content-Type': 'application/zip',
        'Content-disposition': 'attachment; filename=isaactracker.zip'
    });
     
    var zip = archiver('zip');
    zip.pipe(res);
    zip.append('[plugins]\r\ntracker=tracker\r\n\r\n[tracker]\r\nkey=' + key, {name: 'gemini/settings.ini'})
        .append('Copy the files in this directory to your rebirth folder, launch rebirth, start playing.\r\n\r\n:D', {name: 'readme.txt'})
        .bulk([{ expand: true, cwd: process.cwd() + '/public/Release/', src: ['**'], dest: ''}])
        .finalize();
    
};