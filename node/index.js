'use strict';

var http = require('http');
var express = require('express');
var kraken = require('kraken-js');
var fs = require('fs');
var util = require('util');
var log_file = fs.createWriteStream(__dirname + '/isaactracker.log', {flags : 'a'});
var log_stdout = process.stdout;

var options, app, server;

/*
 * Create and configure application. Also exports application instance for use by tests.
 * See https://github.com/krakenjs/kraken-js#options for additional configuration options.
 */
options = {
    onconfig: function (config, next) {
        /*
         * Add any additional config setup or overrides here. `config` is an initialized
         * `confit` (https://github.com/krakenjs/confit/) configuration object.
         */
        next(null, config);
    }
};

app = module.exports = express();
app.use(kraken(options));
app.on('start', function () {
    console.log('Application ready to serve requests.');
    console.log('Environment: %s', app.kraken.get('env:env'));
    console.log('Logging to file: isaactracker.log');
    console.log('use `tail -f isaactracker.log` to follow the log');
    console.log = function (d, stream_key) { //
        if (stream_key != null) {
            fs.appendFile(__dirname + '/user_logs/' + stream_key + '.log', util.format(d) + '\r\n', function (err) {
                if (err) {
                    throw err;
                }
            });
        } else{
            log_file.write(util.format(d) + '\n');
            log_stdout.write(util.format(d) + '\n');
        }
    };
});



process.on('uncaughtException', function(err) {
    if (typeof err === 'object') {
        if (err.message) {
            console.log('\nMessage: ' + err.message)
        }
        if (err.stack) {
            console.log('\nStacktrace:')
            console.log('====================')
            console.log(err.stack);
        }
    } else {
        console.log('uncaughtException: ' + err);
    }
});


/*
 * Create and start HTTP server.
 */
if (!module.parent) {

    /*
     * This is only done when this module is run directly, e.g. `node .` to allow for the
     * application to be used in tests without binding to a port or file descriptor.
     */
    server = http.createServer(app);
    server.listen(process.env.PORT || 7000);
    server.on('listening', function () {
        console.log('Server listening on http://localhost:%d', this.address().port);
    });

}
