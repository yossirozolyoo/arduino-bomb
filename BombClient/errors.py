class BombError(Exception):
    pass


class ProtocolError(BombError):
    pass


class TimeoutError(ProtocolError):
    pass


class BadResponseError(ProtocolError):
    pass
