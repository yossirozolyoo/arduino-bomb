class BombError(Exception):
    pass


class ProtocolError(BombError):
    pass


class TimeoutError(ProtocolError):
    pass


class BadResponseError(ProtocolError):
    pass


class NACKError(BadResponseError):
    pass


class SerialError(BombError):
    pass