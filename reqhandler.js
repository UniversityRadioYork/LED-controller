// Handle Genre create on POST
const zpad = require('zpad');
let currentMode = 'a';
let currentPalette = 'r';
let currentBrightness = 25;
let currentDelay = 20;
exports.getCommands = function (req, res) {

  //Validate arguments
  req.checkBody('mode', 'Mode required').notEmpty();
  req.sanitize('mode').escape();
  req.sanitize('mode').trim();
  req.checkBody('palette', 'Palette required').notEmpty();
  req.sanitize('palette').escape();
  req.sanitize('palette').trim();
  req.checkBody('brightness', 'Invalid brightness').notEmpty().isInt();
  req.checkBody('delay', 'Invalid step delay').notEmpty().isInt();

  let errors = req.validationErrors();

  let cmd = {
    mode: req.body.mode,
    palette: req.body.palette,
    brightness: req.body.brightness,
    delay: req.body.delay
  };

  //Create needed commands from data
  if (errors) {
    res.send(errors);
    return false;
  } else {
    let commands = [];
    //Adds mode command if needed
    if (cmd.mode != currentMode) {
      currentMode = cmd.mode;
      commands.push("M" + cmd.mode);
    };
    //Adds palette command if needed
    if (cmd.palette != currentPalette) {
      currentPalette = cmd.palette;
      commands.push("P" + cmd.palette);
    };
    //Adds brightness command if needed
    if (cmd.brightness != currentBrightness) {
      currentBrightness = cmd.brightness;
      commands.push("B" + zpad(cmd.brightness, 3));
    };
    //Adds delay command if needed
    if (cmd.delay != currentDelay) {
      currentDelay = cmd.delay;
      commands.push("D" + zpad(cmd.delay, 3));
    };

    return commands;
  };
};

//Export current settings for return form
exports.currentSettings = function () {
  return {
    mode: currentMode,
    palette: currentPalette,
    brightness: currentBrightness,
    delay: currentDelay
  }
}