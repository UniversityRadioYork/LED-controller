const SerialPort = require('serialport');
const port = new SerialPort('COM3', {
  baudRate: 9600
})

port.on('error', function (err) {
  console.log('Error: ', err.message)
})

port.on('readable', function () {
  data += port.read()
  if (data.indexOf("\n") > -1) {
    let datas = data.split("\n");
    data = datas[1];
    let validation = datas[0];
    if (validation != last_command) {
      console.log("Command was garbled, resending...")
      sendCmd(last_command, true);
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
      return err || console.log("Sent");
    })
  }
}

module.exports = {
  CMD: sendCmd
};