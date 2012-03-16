var net = require('net');
net.createServer(function (stream) {
    stream.write("Echo server\r\n");
    stream.on("data", function (data) {
        stream.write(data);
    });
    stream.on("end", function () {
        stream.end();
    });
}).listen(8124, "127.0.0.1");
