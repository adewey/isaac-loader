//Start C++ WS socket thingy stuff \o/
var WebSocketServer = require('ws').Server;
var http = require('http');

var server = http.createServer();
var wss = new WebSocketServer({ server: server, path: '/' });
console.log("Dev websockets listening on 8002");
server.listen(8002);

module.exports = wss;