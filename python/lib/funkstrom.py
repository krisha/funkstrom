import serial

VAL_VSS = 0b10001000
VAL_VCC = 0b11101110
VAL_FLW = 0b10001110

RESEND  = 3

class Funkstrom:

    def __init__(self, ser_device, baudrate=31337):
        self.ser = serial.Serial(port=ser_device, baud=baudrate)
        self.ser.open()

    def send_datagram(self, datagram):
        
        #send it multiple times
        for x in xrange(RESEND):
            self.ser.write(datagram)

    def socket_switch(self, syscode, unitcode, on_off):

        self.check_code(syscode)
        self.check_code(unitcode)

        datagram = bytearray()

        for dip_switch in syscode:
            datagram.append(VAL_VSS if dip_switch else VAL_FLW)

        for dip_switch in unitcode:
            datagram.append(VAL_VSS if dip_switch else VAL_FLW)

        # append datagram off/on switch code
        if on_off:
            datagram.extend([VAL_VSS, VAL_FLW])
        else:
            datagram.extend([VAL_FLW, VAL_FLW])
    
        # append stopbit to the end
        datagram.append(0b10000000)

        self.send_datagram(datagram)

    def socket_on(self, syscode, unitcode):
        self.socket_switch(syscode, unitcode, True)

    def socket_off(self, syscode, unitcode):
        self.socket_switch(syscode, unitcode, False)

    def check_code(self, code):
        if type(code) != 'list' or len(code) != 5:
            raise Exception('Code is not a list or not 5 elements long')

    @staticmethod
    def get_syscode(self, S1=False, S2=False, S3=False, S4=False, S5=False):
        return [S1, S2, S3, S4, S5]

    @staticmethod
    def get_unitcode(self, A=False, B=False, C=False, D=False, E=False):
        return [A, B, C, D, E]


if __name__ == '__main__':

    syscode = Funkstrom.get_syscode()
    unitcode = Funkstrom.get_unitcode(D=True)

    fs = Funkstrom('/dev/funkstrom', 31337)
    fs.socket_on(syscode, unitcode)

