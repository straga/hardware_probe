
from ustruct import pack as pk
from ustruct import unpack as upk
from machine import I2C, Pin
i2c = I2C(1, scl=Pin(25), sda=Pin(26), freq=10000)

from ustruct import pack as pk

i2c.writeto(10, pk('b', 0x21))


dat = pk('i', 40)
i2c.writeto_mem(10, 0x32, dat)

# Set power : 30%
i2c.writeto_mem(10, 0x32, pk('i', 30))

# Get power
i2c.writeto(10, pk('b', 0x31)) # ask about power
data = i2c.readfrom(10, 4) # read power value b'\x1e\x00\x00\x00'
upk('b', data) # == 30

