## Edit background.html as you wish, the page needs to meet the steamapi requirements (or Chromium requirements)

# Attention:

1. When the main menu command is called, a function will be requested on the web page with the following signature:

``` typescript
function OnVGUICommand( command : string, cursorPosX : number, cursorPosY : number) : void
```

2. After the web page is loaded, button 0 will be pressed to interact with the web page (useful when playing video or audio automatically)