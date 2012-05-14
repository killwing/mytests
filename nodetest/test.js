#!/usr/local/bin/node



a=1;
var f = function() {
    if (!a) {
        a = 2;
    }
    console.log(a)
};

f()

