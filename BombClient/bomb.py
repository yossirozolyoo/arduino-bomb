import serial
import struct
from errors import *
from datetime import timedelta


COMMANDS = {
    'keep-alive': b'\x60',
    'get-remaining-time': b'\x61'
}
ACK = b'\x70'


DEFAULT_SERIAL_CONFIGURATION = {
    'baudrate': 115200,
    'timeout': 5,
    'write_timeout': 1
}


class Bomb:
    """
    An object for controling a connected bomb through its serial interface.
    """
    def __init__(self, port: str, **kwargs):
        """
        Initialize the `Bomb` object.

        :param port: The port to use to connect to the bomb.
        :param kwargs: 
            Additional configuration to pass to the serial object on initialization
        
        :raises:
            TimeoutError        On timeout while waiting for keep-alive
            BadResponseError
                When any response is returned instead of ACK for the keep-alive
        """
        configuration = DEFAULT_SERIAL_CONFIGURATION.copy()
        configuration.update(kwargs)
        self._serial = serial.Serial(port, **configuration)

        self.keep_alive()

    def _read(self, size: int) -> bytes:
        """
        Read a buffer from serial.

        :param size: The size to read from serial.
        :raises: 
            TimeoutError        On timeout
        :return: The read buffer
        """
        response = self._serial.read(size)
        if len(response) < size:
            raise TimeoutError(f'Timeout reached, recieved {len(response)} out of {size} requested')
        
        return response

    def _read32(self) -> int:
        """
        Reads a 32-bit unsigned number from serial.

        :return: The read value
        """        
        return struct.unpack("I", self._read(4))[0]

    def send_command(self, command: bytes):
        """
        Send a command and wait for the bomb's response

        :raises:
            TimeoutError        On timeout
            BadResponseError    When any response is returned instead of ACK
        """
        self._serial.write(command)

        response = self._read(1)
        if response != ACK:
            raise BadResponseError(f'Recieved 0x{response[0]:02x} instead of ACK on keep-alive')

    def keep_alive(self):
        """
        Send a keep-alive command and wait for the bomb's response

        :raises:
            TimeoutError        On timeout
            BadResponseError    When any response is returned instead of ACK
        """
        self.send_command(COMMANDS['keep-alive'])

    @property
    def left(self) -> timedelta:
        """
        Return the left time for the bomb to explode.

        :return: The left time for the bomb to explode.
        """
        self.send_command(COMMANDS['get-remaining-time'])
        return timedelta(seconds=self._read32())
