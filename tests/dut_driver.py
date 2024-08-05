from serial import Serial
from struct import unpack


class DutDriver(object):
    """Device under test driver."""

    def __init__(self, port: str, baud: int) -> None:
        # Connect to device on init
        self._serial = self._connect(port, baud)
        self.SERIAL_NUMBER_STR_LEN = 11
        self.SIMPLE_REPLY_LEN = 1

    def write_bytes(self, data: str) -> None:
        """Send raw bytes to device.

        Args:
            data (str): data to send.
        """
        self._serial.write(data.encode())

    def read_bytes(self, size: int) -> bytes:
        """Read raw bytes from device.

        Args:
            size (int): amount of bytes to read.

        Returns:
            bytes: received raw data from device.
        """
        return self._serial.read(size)

    def req_serial_num(self) -> str:
        self.write_bytes("0")
        reply = self.read_bytes(self.SERIAL_NUMBER_STR_LEN)
        return reply.decode("ascii")[:-1]

    def req_jumper_status(self) -> int:
        self.write_bytes("1")
        reply = self.read_bytes(self.SIMPLE_REPLY_LEN)
        return unpack("<b", reply)[0]

    def req_button_press(self) -> int:
        self.write_bytes("2")
        reply = self.read_bytes(self.SIMPLE_REPLY_LEN)
        return unpack("<b", reply)[0]

    def _connect(self, port: str, baud: int) -> Serial:
        """Process connection to serial adapter.

        Args:
            port (str): port name.
            baud (int): speed in MBit/s.

        Returns:
            Serial: serial interface handler.
        """
        try:
            return Serial(port=port, baudrate=baud)
        except IOError:
            print("Error open port")
            raise IOError
