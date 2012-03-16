var net = require('net');

con = net.createConnection(8124);
con.on("connect", function(data) {
   console.log('i am connected.');
   con.write('hello world');
});

con.on("data", function(data) {
   console.log('recved: '+data);
   con.end();
});
