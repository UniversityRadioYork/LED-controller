const SerialPort = require('serialport')
const port = new SerialPort('COM4', {baudRate: 9600})

port.on('error', function(err) {
  console.log('Error: ', err.message)
})

// let data = "test"
// port.on('readable', function () {
//   data += port.read()
//   console.log(data)
// })

port.on('data', function (data) {
  console.log('Data:', data.toString('utf8'));
});

function sendCmd(message){
  console.log("sending...")
  port.write(message, function(err) {
    return err || "Success!"
  })
}

module.exports = {CMD: sendCmd}

//setTimeout(function(){console.log(sendCmd("/Mr;"))},6000)