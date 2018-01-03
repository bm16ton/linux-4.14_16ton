from Crypto.Hash import SHA256
from Crypto.Hash import MD5
import sys
import hmac
import struct
import socket
from time import time

def gen_hmac(secret, ip):
    epoch_mins = (long)(time()/60)
    s = hmac.HMAC(secret, digestmod = SHA256)
    s.update(socket.inet_aton(socket.gethostbyname(ip)))
    s.update(struct.pack("i", epoch_mins)) # "i" is for integer
    print s.hexdigest()

if __name__ == '__main__':
	gen_hmac(sys.argv[1], sys.argv[2])
