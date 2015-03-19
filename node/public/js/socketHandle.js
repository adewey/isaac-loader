var items;
socket.on('update', function (data) {
    if (items) {

    } else {
        items = data.items;
        Debug.log("Meep");
    }
});