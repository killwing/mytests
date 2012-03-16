#!/usr/bin/python


def HexToByte( hexStr ):
    """
    Convert a string hex byte values into a byte string. The Hex Byte values may
    or may not be space separated.
    """
    # The list comprehension implementation is fractionally slower in this case    
    #
    #    hexStr = ''.join( hexStr.split(" ") )
    #    return ''.join( ["%c" % chr( int ( hexStr[i:i+2],16 ) ) \
    #                                   for i in range(0, len( hexStr ), 2) ] )
 
    bytes = []

    hexStr = ''.join( hexStr.split(" ") )

    for i in range(0, len(hexStr), 2):
        bytes.append( chr( int (hexStr[i:i+2], 16 ) ) )

    return ''.join( bytes )

f = open('vcard.txt', 'r')
fo = open('vcard_out.txt', 'w')

for line in f:
    index = line.find('UTF-8:')
    if index != -1:
        new = line[index+6:].replace('=','')
        new = new.replace(';', '')
        new = new.replace('\n', '')
        new = new.replace('(', '20')
        new = new.replace(')', '20')
        new = new.strip()
        new = HexToByte(new)
        new = new.decode('utf-8')
        new = line[:index+6] + new + '\r\n'
        fo.write(str(new.encode('utf-8')))
    else:
        fo.write(line)

f.close()
fo.close()




