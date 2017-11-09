const SerialPort = require('serialport');
const port = new SerialPort('COM5', {
  baudRate: 9600
})

port.on('error', function (err) {
  console.log('Error: ', err.message)
})

port.on('readable', function () {
  data += port.read()
  console.log(data)
})

port.on('data', function (data) {
  console.log('Data:', data.toString('utf8'));
});

function sendCmd(message, online) {
  if (online) {
    console.log("sending: ", message);
    port.write(message, function (err) {
      return err || "Success!"
    })
  }
}

module.exports = {
  CMD: sendCmd
};