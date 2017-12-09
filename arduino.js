const SerialPort = require('serialport');
const port = new SerialPort('COM6', {
  baudRate: 9600
})
var need_validation = false;
var cmdqueue = [];
var data = "";

port.on('error', function (err) {
  console.log('Error: ', err.message)
})

port.on('data', function (streamData) {
  data += String(streamData);
  //console.log("Stream:", data)
  if (data.indexOf("\r\n") > -1) {
    let datas = data.split("\r\n");
    data = datas.pop(datas.length-1);
    //console.log(datas, data)
    datas.forEach(handleData);
  }
})


function handleData(h_data){
  //console.log("handling:",h_data)
  if(need_validation){
    if(h_data[0] == "/"){
      let cmd = h_data.slice(1);
      if(cmd.slice(0,last_command.length) == last_command){
        console.log("Command Validated.")
        port.write("#");
      } else {
        //console.log(cmd.slice(0,last_command.length)+" =/= "+ last_command)
        console.log("Command was garbled, resending...")
        cmdqueue.splice(0,0,last_command);
      }
    } else if(h_data == "ERR"){
      console.log("Invalid command");
    }
  } else {
    console.log("DATA:",h_data)
  }
  need_validation = false;
  setTimeout(function(){
    if(cmdqueue.length > 0){
      sendCmd(cmdqueue.pop(0), true);
    }
  }, 300);
}


function sendCmd(message, online) {
  if (online && !need_validation) {
    console.log("Sending: ", message);
    //console.log("    Queue:", cmdqueue);
    last_command = message;
    need_validation = true;
    port.write("/"+message+";", function (err) {
      return err || console.log("Sent");
    })
  } else {
    cmdqueue.push(message);
    //console.log("Queueing up",message);
    //console.log("    Queue:", cmdqueue);
  }
}

module.exports = {
  CMD: sendCmd
};