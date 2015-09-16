function appendHours () {
  var hour
  for (var i = 0; i < 24; ++i) {
    hour = i + ':00'
    $('#startHour').append('<option value="' + i + '">' + hour + '</option>')
    $('#endHour').append('<option value="' + i + '">' + hour + '</option>')
  }
}

function urlParam (name) {
  var results = new RegExp('[\\?&]' + name + '=([^&#]*)').exec( window.location.href)
  if (!results) {
    return 0
  }
  return results[1] || 0
}

function loadUserValues () {
  var weatherUnits = decodeURIComponent( urlParam("KEY_WEATHER_UNITS"))
  var vibration = decodeURIComponent( urlParam("KEY_VIBRATIONS"))
  var startHour = decodeURIComponent( urlParam("KEY_START_HOUR"))
  var endHour = decodeURIComponent( urlParam("KEY_END_HOUR"))

  $('#weatherUnits [value="' + weatherUnits + '"]').prop('selected', true)
  $('#vibration [value="' + vibration + '"]').prop('selected', true)
  $('#startHour [value="' + startHour + '"]').prop('selected', true)
  $('#endHour [value=' + endHour + ']').prop('selected', true)

}

$('#vibration').change(function() {
  if ($('#vibration').val() === 'on') {
    $('#startHour').prop('disabled', false)
    $('#endHour').prop('disabled', false)
  } else {
    $('#startHour').prop('disabled', 'disabled')
    $('#endHour').prop('disabled', 'disabled')
  }
})

$('#saveButton').click(function () {
  var weatherUnits = $('#weatherUnits').val()
  var vibration = $('#vibration').val()
  var startHour = $('#startHour').val()
  var endHour = $('#endHour').val()

  var data = {
    'KEY_WEATHER_UNITS': weatherUnits,
    'KEY_VIBRATIONS': vibration,
    'KEY_START_HOUR': startHour,
    'KEY_END_HOUR': endHour
  }

  document.location = "pebblejs://close#" + encodeURIComponent( JSON.stringify(data))
})

$('#cancelButton').click(function () {
  document.location = 'pebblejs://close'
})

$().ready(function () {

  appendHours()
  loadUserValues()
  $('#vibration').trigger('change')
})
