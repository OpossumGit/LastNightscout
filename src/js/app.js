var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig);

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

  var dir2Char = {
    NONE: ' ',
    DoubleUp: '³/', SingleUp: '²/', FortyFiveUp: '¹/', 
    Flat: '=', 
    FortyFiveDown: '¹\\', SingleDown: '²\\', DoubleDown: '³\\', 
    'NOT COMPUTABLE': ' ', 'RATE OUT OF RANGE': '#'
  };

function directionToChar(direction) {
    return dir2Char[direction] || '-';
  }

function pad(value) {
    if(value < 10) {
        return '0' + value;
    } else {
        return value;
    }
}

function getReadings(url, units) {
  var url_complete = url + '/api/v1/entries/sgv.json';
  var mmol = (units == 'mmol');
  // Send request to Nightscout
  xhrRequest(url_complete, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with nightscout info
      var json = JSON.parse(responseText);
      
      var time1 = pad(new Date(json[0].date).getHours()) + ':' + pad(new Date(json[0].date).getMinutes()); 
      var sgv1;
      if (mmol) {
        sgv1 = (Math.round((json[0].sgv/18)*10)/10).toFixed(1);
      } else {
        sgv1=json[0].sgv;
      }
      var direction1 = directionToChar(json[0].direction);
      
      var time2 = Math.round((json[0].date - json[1].date)/(1000*60));
      var sgv2;
      if (mmol) {
        sgv2 = (Math.round((json[1].sgv/18)*10)/10).toFixed(1);
      } else {
        sgv2=json[1].sgv;
      }
      var direction2 = directionToChar(json[1].direction);
      
      var time3 = Math.round((json[1].date - json[2].date)/(1000*60));
      var sgv3;
      if (mmol) {
        sgv3 = (Math.round((json[2].sgv/18)*10)/10).toFixed(1);
      } else {
        sgv3=json[2].sgv;
      }
      var direction3 = directionToChar(json[2].direction);
      
      var dictionary = {
        'SGV1': sgv1 + ' ' + direction1,
        'TIME1': '@' + time1,
        'SGV2': sgv2 + ' ' + direction2,
        'TIME2': '- ' + time2 + '\'',
        'SGV3': sgv3 + ' ' + direction3,
        'TIME3': '- ' + time3 + '\''
      };
      
      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
      function(e) {
        console.log('Nightscout info sent to Pebble successfully!');
      },
      function(e) {
        console.log('Error sending Nightscout info to Pebble!');
  }
); 
      
    }      
  );
}

// function to inform watch this js is ready
function iamready(){
          Pebble.sendAppMessage({ "READY": "???"},
          function(e) {
            //console.log("Successfully delivered ask for ip message");
          },
          function(e) {
            console.log("Unable to deliver message. Error is: " + e.error.message);
          }
        );
}


// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
    iamready();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
    
    // Get readings
    getReadings(e.payload.URL, e.payload.UNITS);
  }                     
);
