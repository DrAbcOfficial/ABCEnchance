## Edit background.html as you wish, the page needs to meet the steamapi requirements (or Chromium requirements)

# Attention:

1. When the main menu command is called, a event will dispatch to the web page `window` with the following event:

``` javascript
{
    command : "MenuCommand",
    cursor : {
        x : 1,
        y : 2
    }
}
```

2. After the web page is loaded, button 0 will be pressed to interact with the web page (useful when playing video or audio automatically)