#!/usr/local/bin/node

var net = require('net');

var EOL = '\r\n';

var Parser = {
    via: function(msg) {
        var fields = msg.match(/^via: *(sip\/2.0\/tcp +([.\d]+)(:(\d+))?;branch=z9hG4bK(\w+))\r\n/im);
        return {
            str: fields[1],
            host: fields[2],
            port: fields[4],
            branch: fields[5]
        };
    },

    from: function(msg) {
        var fields = msg.match(/^from:.*(<sip:.*>(;tag=(\d+))?)\r\n/im);
        return {
            str: fields[1],
            tag: fields[3]
        };
    },

    to: function(msg) {
        var fields = msg.match(/^to:.*(<sip:.*>(;tag=(\d+))?)\r\n/im);
        return {
            str: fields[1],
            tag: fields[3]
        };
    },

    call_id: function(msg) {
        var fields = msg.match(/^call-id: *(.*)\r\n/im);
        return {
            str: fields[1]
        };
    },

    cseq: function(msg) {
        var fields = msg.match(/^cseq: *((\d+) (\w+))\r\n/im);
        return {
            str: fields[1],
            seq: fields[2],
            method: fields[3]
        };
    },

    contact: function(msg) {
        var fields = msg.match(/^contact: *(<sip:.*>)\r\n/im);
        return {
            str: fields[1]
        };
    },

    max_forwards: function(msg) {
        var fields = msg.match(/^max-forwards: *(\d+)\r\n/im);
        return {
            str: fields[1]
        };
    },
};

var Response = {
    trying: function(req) {
        var str = 'SIP/2.0 100 Trying' + EOL;
        str += 'Via: ' + Parser.via(req).str + EOL;
        str += 'From: ' + Parser.from(req).str + EOL;
        str += 'To: ' + Parser.to(req).str + ';tag=123456' + EOL;
        str += 'Call-ID: ' + Parser.call_id(req).str + EOL;
        str += 'CSeq: ' + Parser.cseq(req).str + EOL;
        str += 'Content-Length: 0' + EOL + EOL;
        return str;
    },

    ringing: function(req) {
        var str = 'SIP/2.0 180 Ringing' + EOL;
        str += 'Via: ' + Parser.via(req).str + EOL;
        str += 'From: ' + Parser.from(req).str + EOL;
        str += 'To: ' + Parser.to(req).str + ';tag=123456' + EOL;
        str += 'Call-ID: ' + Parser.call_id(req).str + EOL;
        str += 'CSeq: ' + Parser.cseq(req).str + EOL;
        str += 'Content-Length: 0' + EOL + EOL;
        return str;
    },

    ok: function(req) {
        var sdp = 'v=0\r\no=- 53655765 2353687637 IN IP4 192.168.16.55\r\ns=Basic Session\r\nc=IN IP4 192.168.16.1\r\nt=0 0\r\nm=audio 56790 RTP/AVP 0 8\r\n\r\n';

        var str = 'SIP/2.0 200 OK' + EOL;
        str += 'Via: ' + Parser.via(req).str + EOL;
        str += 'From: ' + Parser.from(req).str + EOL;
        str += 'To: ' + Parser.to(req).str + ';tag=123456' + EOL;
        str += 'Call-ID: ' + Parser.call_id(req).str + EOL;
        str += 'CSeq: ' + Parser.cseq(req).str + EOL;
        str += 'Contact: <sip:8889103@192.168.16.1;transport=TCP>' + EOL;
        str += 'Content-Type: application/sdp' + EOL;
        str += 'Content-Length: ' + sdp.length + EOL;

        str += EOL;
        str += sdp;
        return str;
    }
};


net.createServer(function(sock) {
    sock.setEncoding('ascii');
    sock.on('data', function(data) {
        console.log('recved:', data);
        if (/^INVITE/im.test(data)) {
            var trying = Response.trying(data);
            sock.write(trying);
            console.log('sent:', trying);

            setTimeout(function() {
                var ringing = Response.ringing(data);
                if (sock.writable) {
                    sock.write(ringing);
                    console.log('sent:', ringing);
                }
            }, 500);

            setTimeout(function() {
                var ok = Response.ok(data);
                if (sock.writable) {
                    sock.write(ok);
                    console.log('sent:', ok);
                }

                sock.destroy();
                console.log('sock end/destroy');
            }, 1000);
        }
    });
    sock.on('end', function() {
        sock.end();
    });
    sock.on('error', function() {
        console.log('sock error');
    });
    sock.on('close', function(had_error) {
        console.log('sock close', had_error);
    });
}).listen(5061, '192.168.16.55');


/*
INVITE sip:8889103@192.168.16.1:5060;transport=TCP;user=phone SIP/2.0 
Via: SIP/2.0/TCP 192.168.16.55:5061;branch=z9hG4bK201106171025122551
From: <sip:+8609292@192.168.16.55;user=phone>;tag=152217585
To: <sip:8889103@192.168.16.1;user=phone>
Call-ID: 6553520110617102423375@192.168.16.55
CSeq: 1 INVITE
Contact: <sip:192.168.16.55:5061;transport=TCP>
Allow: INVITE, ACK, CANCEL, BYE, OPTIONS, PRACK, UPDATE, SUBSCRIBE, NOTIFY, PUBLISH, INFO
Accept: application/sdp
Supported:100rel
Max-Forwards: 70
P-Asserted-Identity: <sip:+8609292@192.168.16.55;user=phone>
P-Charging-Vector: icid-value=4dfabb085278300065535;icid-generated-at=192.168.16.55;charging-info="(PSTN:orig-fni=iamFNI)"
Content-Length:  251
Content-Type: application/sdp

v=0
o=- 1308277512 1308277512 IN IP4 192.168.16.55
s=Basic Session
c=IN IP4 192.168.16.55
t=0 0
m=audio 20000 RTP/AVP 18 8 0 4 96 99
a=rtpmap:96 AMR/8000
a=fmtp:96 octet-align=1
a=rtpmap:99 telephone-event/8000
a=fmtp:99 0-15
a=ptime:20000

SIP/2.0 100 Trying\r\n\
Via: SIP/2.0/TCP 192.168.16.55:5061;branch=z9hG4bK201106161625411021\r\n\
From: <sip:+8609292@192.168.16.55;user=phone>;tag=99411473\r\n\
To: <sip:8889103@192.168.16.1:5060;transport=TCP;user=phone>;tag=123\r\n\
Call-ID: 6553520110616162456798@192.168.16.55\r\n\
CSeq: 1 INVITE\r\n\
Content-Length: 0\r\n";

var invite = 'INVITE sip:8889103@192.168.16.1:5060;transport=TCP;user=phone SIP/2.0\r\nVia: SIP/2.0/TCP 192.168.16.55:5061;branch=z9hG4bK201106171025122551\r\nFrom: <sip:+8609292@192.168.16.55;user=phone>;tag=152217585\r\nTo: <sip:8889103@192.168.16.1;user=phone>\r\nCall-ID: 6553520110617102423375@192.168.16.55\r\nCSeq: 1 INVITE\r\nContact: <sip:192.168.16.55:5061;transport=TCP>\r\nAllow: INVITE, ACK, CANCEL, BYE, OPTIONS, PRACK, UPDATE, SUBSCRIBE, NOTIFY, PUBLISH, INFO\r\nAccept: application/sdp\r\nSupported:100rel\r\nMax-Forwards: 70\r\nP-Asserted-Identity: <sip:+8609292@192.168.16.55;user=phone>\r\nP-Charging-Vector: icid-value=4dfabb085278300065535;icid-generated-at=192.168.16.55;charging-info="(PSTN:orig-fni=iamFNI)"\r\nContent-Length:  251\r\nContent-Type: application/sdp\r\n\r\nv=0\r\no=- 1308277512 1308277512 IN IP4 192.168.16.55\r\ns=Basic Session\r\nc=IN IP4 192.168.16.55\r\nt=0 0\r\nm=audio 20000 RTP/AVP 18 8 0 4 96 99\r\na=rtpmap:96 AMR/8000\r\na=fmtp:96 octet-align=1\r\na=rtpmap:99 telephone-event/8000\r\na=fmtp:99 0-15\r\na=ptime:20000\r\n';
console.log(Response.trying(invite));
*/
