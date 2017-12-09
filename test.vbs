CreateObject("Wscript.Shell").Run "node index.js", 0
CreateObject("Shell.Application").ShellExecute "chrome.exe", "http://localhost:3000", "", "", 1