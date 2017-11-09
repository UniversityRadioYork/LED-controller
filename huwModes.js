const request = require('request');
const ColorToHex = require('colornames');
const serial = require('./arduino.js');
const studio_colors = ["", "#FF0000", "#00FF00", "#0000FF"];
let song_color = false;
let studio_color = false;
let current_color = "#FF00FF";

function getCurrentSongcolor() {
  //Get current song data
  request('http://ury.org.uk/audio/status-json.xsl', function (error, response, body) {
    let data = JSON.parse(body);
    let title = data.icestats.source[5].title;
    //Split title into words
    let words = title.split(" ");
    //look for colors
    for (let i = 0; i < words.length; i++) {
      let found_color = ColorToHex(words[i]);
      if (found_color != undefined) {
        song_color = found_color;
        break;
      } else {
	song_color = false;
      }
    }
  });
}

function getCurrentStudiocolor() {
  //Get studio selector status
  request('https://ury.org.uk/api/v2/selector/statusattime?api_key=9C4KCqywpDfzIk7OEhYO3tOjDJWftg2sZ65fKT5fTGCWvshnz5tinVt1MiqvETM4eZYDtQbRs13GoTCNB8HTsmQQlcDwFmRo8Xw3uHQoycYkumyTVGdXbxtt1S2Ow7RFbK', function (error, response, body) {
    let data = JSON.parse(body);
    studio = data.payload.studio;
    //Lookup the color for the currently on-air studio
    if (0 < studio <= 3 && studio % 1 == 0) {
      studio_color = studio_colors[studio];
    } else {
      bsong_color = false;
    }
  });
}

function pollColor() {
  getCurrentSongcolor();
  getCurrentStudiocolor();
};

exports.DynamicStaticPolling = function (enable, online) {
  pollColor();
  console.log(song_color, studio_color, current_color)
  
  let new_color = false

  if(song_color != false)
  {
    new_color = song_color
  }
  else
  {
    new_color = studio_color
  }

  if (current_color != new_color && enable) {
    current_color = new_color;
    if(!current_color)
    {
      current_color = "#FFFF00";
    }
    serial.CMD("/C" + current_color.slice(1) + ";", online);
  }
}