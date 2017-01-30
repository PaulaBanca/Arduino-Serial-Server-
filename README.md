A simple server that passes data recieved through sockets to the specified Arduino device. 

Uses [Tod E. Kurt's Arduino Serial library](https://github.com/todbot/arduino-serial/) for the Arduino communication and [Silver Moon's C Sockets tutoial](http://www.binarytides.com/socket-programming-c-linux-tutorial/) for the network side of things. 

The envisioned usage is for communicating with an Arduino from within an environment where you are limited to network requests.  

The program assumes a HOME variable and is only tested on Mac OS. Though should work on Linux systems too.

Usage
------
<pre>
laptop%  ./arduino-serial-server
Usage: arduino-serial-server -b <bps> -p <serverport> -a <serialport> [OPTIONS]

Options:
  -h, --help                 Print this help message
  -b, --baud=baudrate        Baudrate (bps) of Arduino (default 9600)
  -a, --arduino=serialport   Serial port Arduino is connected to
  -p, --serverport=port      The port to listen to connections on

Note: Order is important. Set '-b' baudrate before opening port'-p'.
</pre>

