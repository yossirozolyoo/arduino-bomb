import serial
import struct
from typing import Union
from errors import *
from datetime import timedelta


COMMANDS_NAMES = [
    'keep-alive',
    'get-remaining-time',
    'set-remaining-time',
    'get-level',
    'set-level'
]


COMMANDS = {name: bytes([i]) for i, name in enumerate(COMMANDS_NAMES)}


LEVELS = {
    'uart': b'U',
    'spi': b'S',
    'i2c': b'I',
    'done': b'D',
    'fail': b'F'
}


ACK = b'\x70'
NACK = b'\x71'


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
        
        try:
            self._serial = serial.Serial(port, **configuration)
        except (OSError, serial.SerialException):
            raise SerialError(f'Cannot connect to serial port {port} with given configuration.')

        self._wait_for_ack()

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
    

    def _write32(self, value: int):
        """
        Writes a 32-bit unsigned number to the serial.

        :param value: The value to write
        """        
        self._serial.write(struct.pack("I", value))

    @property
    def port(self) -> str:
        """
        Return the COM port the bomb is connected to
        """
        return self._serial.port

    def _wait_for_ack(self):
        """
        Wait for ACK sent by the bomb

        :raises:
            TimeoutError        On timeout
            BadResponseError    When any response is returned instead of ACK
        """
        response = self._read(1)
        if response == NACK:
            raise NACKError()
        elif response != ACK:
            raise BadResponseError(f'Recieved 0x{response[0]:02x} instead of ACK')

    def send_command(self, command: bytes):
        """
        Send a command and wait for the bomb's response

        :raises:
            TimeoutError        On timeout
            BadResponseError    When any response is returned instead of ACK
        """
        self._serial.write(command)
        self._wait_for_ack()

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

    @left.setter
    def left(self, value: Union[int, timedelta]):
        """
        Set the time left on the bomb

        :param value: An `int` or a `timedelta` object representing the left time on the bomb
        """
        if isinstance(value, timedelta):
            value = value.total_seconds()

        self.send_command(COMMANDS['set-remaining-time'] + struct.pack("I", value))

    @property
    def level(self) -> str:
        """
        Get the current level of the user
        """
        self.send_command(COMMANDS['get-level'])
        raw_level = self._read(1)

        for level, raw_encoding in LEVELS.items():
            if raw_encoding == raw_level:
                return level
            
        raise BadResponseError(f'Unexpected encoding of level {repr(raw_level)}')

    @level.setter
    def level(self, value: str):
        """
        Set the current level of the user
        """
        if value not in LEVELS:
            raise ValueError(f'"{value}" is not a supported level')
        
        self.send_command(COMMANDS['set-level'] + LEVELS[value])
