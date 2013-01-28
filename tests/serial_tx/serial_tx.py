import serial

serial_dev = '/dev/funkstrom'
serial_baud = 31337

def main():
    s = serial.Serial(port=serial_dev, baud=serial_baud)
    s.open()

    while True:
        for byte in [0xAA, 0x55]:
            s.write(byte)

if __name__ == '__main__':
    main()
