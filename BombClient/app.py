import json
from flask import Flask, request
from bomb import list_bombs
from errors import *
from typing import Union, Tuple


bomb = list_bombs()[0]
app = Flask(__name__)


def set_left() -> Union[Tuple[str, int], None]:
    """
    Sets the left seconds on the bomb to the user's given value

    :return: The HTTP response if failed
    """
    value = request.form['left']
    if not isinstance(value, int):
        try:
            value = int(value)
        except ValueError:
            return f'Cannot interpret "{value}" as an integer'
    
    try:
        bomb.left = value
    except TimeoutError as err:
        return str(err), 504
    except BombError as err:
        return str(err), 500
    

def set_level() -> Union[Tuple[str, int], None]:
    """
    Sets the current level on the bomb.

    :return: The HTTP response if failed
    """
    value = request.form['level']
    try:
        bomb.level = value
    except TimeoutError as err:
        return str(err), 504
    except BombError as err:
        return str(err), 500
    except ValueError as err:
        return str(err), 400


def set_level_queue() -> Union[Tuple[str, int], None]:
    """
    Sets the current level queue on the bomb.
    
    :return: The HTTP response if failed
    """
    value = request.form.getlist('queue')
    try:
        bomb.level_queue = value
    except TimeoutError as err:
        return str(err), 504
    except BombError as err:
        return str(err), 500
    except ValueError as err:
        return str(err), 400


ACTIONS = {
    'left': set_left,
    'level': set_level,
    'queue': set_level_queue,
}


@app.route('/status', methods=['GET', 'POST'])
def status():
    if request.method == 'POST':
        # Handle POST
        for field in request.form:
            if field not in ACTIONS:
                return f'Unsupported field "{field}"', 400
            
            response = ACTIONS[field]()
            if response is not None:
                return response

    elif request.method != 'GET':
        # Unsupported method
        return 'Method not allowd', 405

    try:
        return {'left': int(bomb.left.total_seconds()), 'level': bomb.level, 'queue': bomb.level_queue}
    except TimeoutError as err:
        return str(err), 504
    except BombError as err:
        return str(err), 500
