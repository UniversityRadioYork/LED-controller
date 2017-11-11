const SerialPort = require('serialport');
const port = new SerialPort('COM3', {
  baudRate: 9600
})
var data = ""
var need_validation = false;

port.on('error', function (err) {
  console.log('Error: ', err.message)
})

port.on('readable', function () {
  data += port.read()
  if (data.indexOf("\n") > -1) {
    let datas = data.split("\n");
    data = datas[1];
    let chunk = datas[0];
    console.log("Recieved:", chunk)
    if (need_validation && chunk != last_command) {
      console.log("Command was garbled, resending...")
      sendCmd(last_command, true);
    } else {
      console.log("Command Validated, sending valid signal")
      sendCmd("#", true);
      need_validation = false;
    }
  }

  console.log(data)
})

// port.on('data', function (data) {
//   console.log('Data:', data.toString('utf8'));
// });

async function sendCmd(message, online) {
  if (online) {
    console.log("Sending: ", message);
    last_command = message;
    port.write(message, function (err) {
      need_validation = true;
      return err || console.log("Sent");
    })
  }
}

module.exports = {
  CMD: sendCmd
};