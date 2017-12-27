# i2c-adapter-ch431a
Linux kernel module for CH341A as USB to I2C bridge.

## Usage example
```
make
sudo insmod i2c-ch341a.ko

sudo modprobe at24
sudo sh -c "echo 24c32 0x50 > /sys/bus/i2c/devices/i2c-0/new_device"
sudo hexdump -Cv /sys/bus/i2c/devices/0-0050/eeprom | less
```

