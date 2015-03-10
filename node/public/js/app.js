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
        $('#tearrate-stat').attr("class","stat tearrate-" + data.tearrate);
    }
    if(data.hasOwnProperty('damage')){
        $('#damage-stat').attr("class","stat damage-" + data.damage);
    }
    if(data.hasOwnProperty('range')){
        $('#range-stat').attr("class","stat range-" + data.range);
    }
    if(data.hasOwnProperty('shotspeed')){
        $('#shotspeed-stat').attr("class","stat shotspeed-" + data.shotspeed);
    }
    if(data.hasOwnProperty('speed')){
        $('#speed-stat').attr("class","stat speed-" + data.speed);
    }
    if(data.hasOwnProperty('luck')){
        $('#luck-stat').attr("class", "stat luck-" + data.luck);
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
    if(data.hasOwnProperty('seed')){
        $('#seed').html(data.seed);
    }
    if(data.hasOwnProperty('updated_at')){
        $('#updated').html(data.updated_at);
    }
    makeToolTip();
}