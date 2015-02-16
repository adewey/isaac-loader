'use strict';
function updateData(data) {
    console.log(data);
    if(data.charImgName){
        $('#charImgName').attr("src","/gfx/ui/stage/" + data.charImgName);
    }
    if(data.hasOwnProperty('coins')){
        $('.coins').html(data.coins);
    }
    if(data.hasOwnProperty('bombs')){
        $('.bombs').html(data.bombs);
    }
    if(data.hasOwnProperty('keys')){
        $('.keys').html(data.keys);
    }
    if(data.hasOwnProperty('tearrate')){
        var barsUsed = 0;
        if (data.tearrate > 7){
            barsUsed = 1;
        }else if(data.tearrate > 5){
            barsUsed = 2;
        }else if(data.tearrate > 3){
            barsUsed = 4;
        }else{
            barsUsed = 7;
        }
        $('#tearrate-stat').attr("class","stat tearrate-" + barsUsed);
    }
    if(data.hasOwnProperty('damage')){
        var barsUsed = 0;
        if (data.damage < 3){
            barsUsed = 1;
        }else if(data.damage< 4.5){
            barsUsed = 2;
        }else if(data.damage < 6.0){
            barsUsed = 3;
        }else if(data.damage < 7.5){
            barsUsed = 4;
        }else if(data.damage < 9){
            barsUsed = 5;
        }else if(data.damage < 10.5){
            barsUsed = 6;
        }else{
            barsUsed = 7;
        }
        $('#damage-stat').attr("class","stat damage-" + barsUsed);
    }
    if(data.hasOwnProperty('range')){
        var barsUsed = 0;
        if (data.range < 0){
            barsUsed = 1;
        }else if(data.range < .5){
            barsUsed = 2;
        }else if(data.range < 1.0){
            barsUsed = 3;
        }else if(data.range < 1.5){
            barsUsed = 4;
        }else if(data.range < 2){
            barsUsed = 5;
        }else if(data.range < 2.5){
            barsUsed = 6;
        }else{
            barsUsed = 7;
        }
        $('#range-stat').attr("class","stat range-" + barsUsed);
    }
    if(data.hasOwnProperty('shotspeed')){
        var barsUsed = 0;
        if (data.shotspeed < 1){
            barsUsed = 1;
        }else if(data.shotspeed < 1.2){
            barsUsed = 2;
        }else if(data.shotspeed < 1.0){
            barsUsed = 3;
        }else if(data.shotspeed < 1.3){
            barsUsed = 4;
        }else if(data.shotspeed < 1.6){
            barsUsed = 5;
        }else if(data.shotspeed < 1.8){
            barsUsed = 6;
        }else{
            barsUsed = 7;
        }
        $('#shotspeed-stat').attr("class","stat shotspeed-" + barsUsed);
    }
    if(data.hasOwnProperty('speed')){
        var barsUsed = 0;
        if (data.speed < 1){
            barsUsed = 1;
        }else if(data.speed < 1.5){
            barsUsed = 2;
        }else if(data.speed < 2){
            barsUsed = 3;
        }else if(data.speed < 2.5){
            barsUsed = 4;
        }else if(data.speed < 3){
            barsUsed = 5;
        }else if(data.speed < 3.5){
            barsUsed = 6;
        }else{
            barsUsed = 7;
        }
        $('#speed-stat').attr("class","stat speed-" + barsUsed);
    }
    if(data.hasOwnProperty('luck')){
        var barsUsed = 0;
        if (data.luck < 2){
            barsUsed = 1;
        }else if(data.luck < 3){
            barsUsed = 2;
        }else if(data.luck < 4){
            barsUsed = 3;
        }else if(data.luck < 5){
            barsUsed = 4;
        }else if(data.luck < 6){
            barsUsed = 5;
        }else if(data.luck < 7){
            barsUsed = 6;
        }else{
            barsUsed = 7;
        }
        $('#luck-stat').attr("class", "stat luck-" + barsUsed);
    }
    if(data.trinkets){
        $('.trinket-list').html('');
        $.each(data.trinkets, function(){
            if(data.trinkets[0]){
                $('.trinket-list').html('<li><img src="/gfx/items/trinkets2x/' + data.trinkets[0].gfx + '" /></li>')
            }
        });
    }
    if(data.items){
        var tempHolder = "";
        $.each(data.items, function(k,v){
            tempHolder += "<li class='" + data.items[k].state + "'>";
            tempHolder += '<img src="/gfx/items/collectibles2x/' + data.items[k].gfx + '"  data-toggle="tooltip" data-html="true" data-placement="bottom" title="<h4>' + data.items[k].name + '(<i class=\'text-danger\'>' + data.items[k].id + '</i>) [<i class=\'text-info\'>' + data.items[k].state + '</i>]</h4><h5>' + data.items[k].description + '</h5>" />';
            tempHolder += "</li>";
        });
        $('.item-list').html(tempHolder);
    }
    if (data.hasOwnProperty('floor')) {
        $('.floor').removeClass('in');
        $( ".floor-list li:nth-child(" + data.floor + ")" ).addClass('in');
        if (data.hasOwnProperty('altfloor')) {
            if(data.altfloor == "1"){
                $( ".floor-list li:nth-child(" + data.floor + ")" ).addClass('alt');
            }else{
				
                $( ".floor-list li:nth-child(" + data.floor + ")" ).removeClass('alt');  
            }
        }
        if (data.curses[2]) {
            $( ".floor-list li:nth-child(" + data.floor + ")" ).addClass('xl');  
        }else{
            $( ".floor-list li:nth-child(" + data.floor + ")" ).removeClass('xl');  
        }
    }
    if(data.hasOwnProperty('guppy')){
        $('.guppyProgress').attr("src","/gfx/IsaacTracker/guppyProgress/" + data.guppy + "_3.png");
    }
    if(data.hasOwnProperty('lof')){
        $('.flyProgress').attr("src","/gfx/IsaacTracker/flyLordProgress/" + data.lof + "_3.png");
    }
    makeToolTip();
}