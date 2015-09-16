var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function locationSuccess(pos) {
  //Weather request
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
            pos.coords.latitude + '&lon=' + pos.coords.longitude;

  xhrRequest(url, 'GET',
    function(responseText) {
      var json = JSON.parse(responseText);
      var temperature = Math.round(json.main.temp - 273.15);

      var dictionary = {
        'KEY_TEMPERATURE': temperature,
      };

      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log('Weather info sent to Pebble');
        },
        function(e) {
          console.log('Error sending the weather to the Pebble');
        }
      );
    }
  );
}

function locationError(err) {
  console.log('Error requesting location');
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

Pebble.addEventListener('ready', function(e) {
  console.log('PebbleKit JS is ready');
  getWeather();
});

Pebble.addEventListener('appmessage', function(e) {
  console.log('AppMessage received');
  getWeather();
});

Pebble.addEventListener('showConfiguration', function(e) {
  var data = JSON.parse(window.localStorage.getItem('pebblePixelGrassData'))
  var configURL = 'http://florenciatarditti.me/pebble-pixel-grass/'
  if (data !== null) {
    configURL +=  '?&KEY_WEATHER_UNITS=' + encodeURIComponent(data['KEY_WEATHER_UNITS']) +
                  '&KEY_VIBRATIONS=' + encodeURIComponent(data['KEY_VIBRATIONS']) +
                  '&KEY_START_HOUR=' + encodeURIComponent(data['KEY_START_HOUR']) +
                  '&KEY_END_HOUR=' + encodeURIComponent(data['KEY_END_HOUR'])
  }
  Pebble.openURL(configURL)
})

Pebble.addEventListener('webviewclosed', function(e) {
  var data = JSON.parse(decodeURIComponent(e.response))
  if (data != undefined) {
    window.localStorage.setItem('pebblePixelGrassData', JSON.stringify(data))
  }
  Pebble.sendAppMessage(data, function(e) {
      console.log("JSON data sent to Pebble")
    }, function(e) {
      console.log("JSON data not sent to Pebble. Error message: " + e.error.message)
    }
  )
})
