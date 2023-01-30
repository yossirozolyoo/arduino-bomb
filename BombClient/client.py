import argparse
from bomb import Bomb
from typing import Callable
from functools import wraps


def connect(func: Callable[[Bomb, argparse.Namespace], None]) -> Callable[[argparse.Namespace], None]:
    """
    A decorator to connect to a running bomb prior entering a function

    :param func: 
        The function to wrap. This function must recieve as its only arguments
        a `Bomb` instance and an `argparse.Namespace` instance.
    :return: 
        The wrapped function, which does not recieve a `Bomb` instance as an
        argument.
    """
    @wraps(func)
    def wrapped(args: argparse.Namespace):
        if args.port is None:
            bomb = None

        else:
            bomb = Bomb(args.port)

        return func(bomb, args)

    return wrapped


@connect
def operation_ipython(bomb: Bomb, args: argparse.Namespace):
    """
    An implementation for the user command 'ipython'.

    :param bomb: The connected bomb instance
    :param args: The arguments supplied by the user.
    """
    # Import the package here to support python interpreters without the
    # IPython module.
    import IPython
    IPython.embed()


def main():
    parser = argparse.ArgumentParser(description="Bomb management CLI")
    parser.add_argument('-p', '--port', default=None, help= \
        'The serial port to connect to. When ommited, the serial port ' + \
        'is found automatically.')

    subparsers = parser.add_subparsers(required=True, help="Operations")

    # IPython parser
    ipython_parser = subparsers.add_parser('ipython', help= \
        'Open an IPython shell with an object named `bomb` which controls' + \
        ' the bomb')
    ipython_parser.set_defaults(operation=operation_ipython)

    args = parser.parse_args()
    args.operation(args)


if __name__ == '__main__':
    main()
