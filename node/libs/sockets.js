var app = require('express')(),
    http = require('http').Server(app),
    io = require('socket.io')(http);

io.on('connection', function(socket){
    var Room;
    socket.on('join', function(room){
        Room = room;
        if (Room)
        {
            socket.join(Room);
        }
    });
    socket.on('disconnect', function(){
        if (Room)
        {
            socket.leave(Room);
            Room = undefined;
        }
    });
});

module.exports = io;
console.log("Dev Sockets Listening on 8001");
http.listen(8001);