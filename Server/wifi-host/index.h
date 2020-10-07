const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<html>
  <head>
    <title>NodeMCU Wifi Car</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
  </head>
  <body>
    <div align="center">
    <h1>NodeMCU Wifi Car</h1>
    <br>
    <form method="GET">
    <input type="button" value="Go forward" onclick="window.location.href='forward'">
    <br><br>
    <input type="button" value="Go backward" onclick="window.location.href='backward'">
    <br><br>
    <input type="button" value="Turn left" onclick="window.location.href='left'">
    <br><br>
    <input type="button" value="Turn right" onclick="window.location.href='right'">
    <br><br>
    <input type="button" value="Car stop" onclick="window.location.href='stop'">
    </form>
    </div>
  </body>
</html>)rawliteral";
