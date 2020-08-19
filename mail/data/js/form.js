// a modifier avec le lien exacte du fichier
$.getJSON( "time.json", function( data ) {
  var items = [];
  $.each( data, function( key, val ) {
  // console.log(key, val["h"] + ":" + val["m"]);
    if (val["h"] == null) {val["h"]='--'}
    if (val["m"] == null) {val["m"]='--'}
    items.push( "<tr><td>" + key + "</td> <td>" + val["h"] + ":" + val["m"] + "</td></tr>" );
  });
  $("#horraire").append(items);
});

Array.prototype.sortOn = function(key){
    this.sort(function(a, b){
        if(a[key] < b[key]){
            return -1;
        }else if(a[key] > b[key]){
            return 1;
        }
        return 0;
    });
}

$.getJSON( "takeMed.json", function( data ) {
  var items = [];
  $.each( data['takeMed'], function( key, val ) {
    $.each( val, function( key1, val1 ) {
        if (val1['year']) {
            items.push({'message': key, 'date': val1['day']+'-'+val1['month']+'-'+val1['year']+' '+val1['hour']+':'+val1['min']+':'+val1['sec']})    
        }
  });
  });
  items.sortOn("date");
  $.each(items, function( key, val ){
    items[key] = "<tr><td>" + val['message'] + "</td> <td>" + val['date']+ "</td></tr>"
  });
  // items.push("<tr><td>" + key + "</td> <td>" + val1['day']+'-'+val1['month']+'-'+val1['year']+' '+val1['hour']+':'+val1['min']+':'+val1['sec']+ "</td></tr>")
  // console.log(items)
  $("#historique").append(items);
});


$('#alertsuccess').hide();
$('#calculerButton').click(function( event ) {
    event.preventDefault();

    $.ajax({
        method: "POST",
        url:"/post", //"formulaire.php", 
        data: { matin: $('#matin').val(), midi: $('#midi').val(), apres_midi : $('#apres_midi').val(), soir: $('#soir').val() },
        success: function(result){
            // $('#alert-flash').text("Nouveaux horaires bien enregistrés.");
            // $('#alertsuccess').show();
            location.reload();
      }});
});