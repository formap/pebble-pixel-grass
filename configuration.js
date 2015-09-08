function appendHours() {
  var hour
  for (var i = 0; i < 24; ++i) {
    hour = i + ':00'
    $('#startHour').append('<option value="' + hour + '">' + hour + '</option>')
    $('#endHour').append('<option value="' + hour + '">' + hour + '</option>')
  }
}

function loadUserValues() {
  // load the user values
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

$('#saveButton').click(function() {
  var vibrations = $('#vibrationCheckbox').prop('checked')
  var startHour
  var endHour
  if (vibrations) {
    startHour = $('#startHour').val()
    endHour = $('#endHour').val()
  }

  var data = {
    'KEY_VIBRATIONS': vibrations,
    'KEY_START_HOUR': startHour,
    'KEY_END_HOUR': endHour
  }

  document.location = 'pebblejs://close#' + encodeURIComponent(JSON.stringify(data))
})

$('#cancelButton').click(function() {
  document.location = 'pebblejs://close'
})

$(document).ready(function() {
  loadUserValues()
  appendHours()
});
