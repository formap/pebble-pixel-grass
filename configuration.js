function urlParam (name) {
  var results = new RegExp('[\\?&]' + name + '=([^&#]*)').exec( window.location.href)
  if (!results) {
    return 0
  }
  return results[1] || 0
}

function setSelectValue (id, value) {
  var element = document.getElementById(id)
  element.value = value
}

function disableSelect (id, value) {
  document.getElementById(id).disabled = value
}

function loadUserValues () {
  var weatherUnits = decodeURIComponent( urlParam("KEY_WEATHER_UNITS"))
  weatherUnits = (weatherUnits === undefined) ? weatherUnits : 'c'
  var vibration = decodeURIComponent( urlParam("KEY_VIBRATIONS"))
  vibration = (vibration === undefined) ? vibration : 'off'
  var startHour = decodeURIComponent( urlParam("KEY_START_HOUR"))
  startHour = (startHour === undefined) ? startHour : 9
  var endHour = decodeURIComponent( urlParam("KEY_END_HOUR"))
  endHour = (endHour === undefined) ? endHour : 22

  setSelectValue('weatherUnits', weatherUnits)
  setSelectValue('vibration', vibration)
  setSelectValue('startHour', startHour)
  setSelectValue('endHour', endHour)
}

function onVibrationChanged () {
  if (document.getElementById('vibration').value === 'on') {
    disableSelect('startHour', false)
    disableSelect('endHour', false)
  } else {
    disableSelect('startHour', true)
    disableSelect('endHour', true)
  }
}

function saveButton () {
  var weatherUnits = document.getElementById('weatherUnits').value
  var vibration = document.getElementById('vibration').value
  var startHour = document.getElementById('startHour').value
  var endHour = document.getElementById('endHour').value

  var data = {
    'KEY_WEATHER_UNITS': weatherUnits,
    'KEY_VIBRATIONS': vibration,
    'KEY_START_HOUR': startHour,
    'KEY_END_HOUR': endHour
  }

  document.location = "pebblejs://close#" + encodeURIComponent( JSON.stringify(data))
}

function cancelButton () {
  document.location = 'pebblejs://close'
}

(function () {
  loadUserValues()
  onVibrationChanged()
})()
