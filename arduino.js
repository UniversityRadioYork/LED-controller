const SerialPort = require('serialport');
const port = new SerialPort('COM3', {
  baudRate: 9600
})
var need_validation = false;
var cmdqueue = [];

port.on('error', function (err) {
  console.log('Error: ', err.message)
})

port.on('data', function (data) {
  //let data = String(port.read());
  data = String(data);
  console.log("Stream:", data)
  if (data.indexOf("\r\n") > -1) {
    let datas = data.split("\r\n");
    datas.forEach(handleData);
  }
})


function handleData(data){
  if(data == '') return;
  console.log(data)
  if(need_validation){
    if(data[0] == "/"){
      let cmd = data.slice(1);
      if(cmd.slice(0,last_command.length) == last_command){
        console.log("Command Validated, sending valid signal")
        port.write("#");
      } else {
        console.log(cmd+";"+ last_command)
        console.log("Command was garbled, resending...")
        cmdqueue.splice(0,0,last_command);
      }
    } else if(data == "ERR"){
      console.log("Invalid command");
    }
  } else {
    console.log("DATA:",data)
  }
  need_validation = false;
  if(cmdqueue.length > 0){
    sendCmd(cmdqueue.pop(0), true);
  }
}


function sendCmd(message, online) {
  if (online && !need_validation) {
    console.log("Sending: ", message);
    console.log("    Queue:", cmdqueue);
    last_command = message;
    need_validation = true;
    port.write("/"+message+";", function (err) {
      return err || console.log("Sent");
    })
  } else {
    cmdqueue.push(message);
    console.log("Queueing up",message);
    console.log("    Queue:", cmdqueue);
  }
}

module.exports = {
  CMD: sendCmd
};