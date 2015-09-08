function appendHours () {
  var hour
  for (var i = 0; i < 60; ++i) {
    hour = i + ':00'
    $('#startHour').append('<option value="' + i + '">' + hour + '</option>')
    $('#endHour').append('<option value="' + i + '">' + hour + '</option>')
  }
}

function urlParam (name) {
  var results = new RegExp('[\\?&]' + name + '=([^&#]*)').exec(window.location.href);
  if (!results) {
    return 0;
  }
  return results[1] || 0;
}

$('#vibrationCheckbox').change(function() {
  if (this.checked) {
    $('#startHour').prop('disabled', false)
    $('#endHour').prop('disabled', false)
  } else {
    $('#startHour').prop('disabled', 'disabled')
    $('#endHour').prop('disabled', 'disabled')
  }
}).trigger('change')

function updateCheckbox (vibrations) {
  $('#vibrationCheckbox').prop('checked', vibrations)
}

function updateHours (startHour, endHour) {
  $('#startHour [value="' + startHour + '"]').prop('selected', true)
  $('#endHour [value=' + endHour + ']').prop('selected', true)
}

function loadUserValues () {
  var vibrations = decodeURIComponent( urlParam("KEY_VIBRATIONS"))
  var startHour = decodeURIComponent( urlParam("KEY_START_HOUR"))
  var endHour = decodeURIComponent( urlParam("KEY_END_HOUR"))

  updateCheckbox(vibrations)
  $('#vibrationCheckbox').change()
  updateHours(startHour, endHour)
}

var data

$('#saveButton').click(function () {
  $('.logs').append('save button pressed')

  var vibrations = $('#vibrationCheckbox').prop('checked')
  var startHour = $('#startHour').val()
  var endHour = $('#endHour').val()

  data = {
    'KEY_VIBRATIONS': vibrations,
    'KEY_START_HOUR': startHour,
    'KEY_END_HOUR': endHour
  }
  $('.logs').append('data -> vibrations: ' + data.KEY_VIBRATIONS + ' startHour: ' + data.KEY_START_HOUR + ' endHour: ' + data.KEY_END_HOUR + ' ')

  document.location = 'pebblejs://close#' + encodeURIComponent(JSON.stringify(data))
})

$('#cancelButton').click(function () {
  document.location = 'pebblejs://close'
})

$(document).ready(function () {
  appendHours()
  loadUserValues()
});
