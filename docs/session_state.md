# JSON
* **state** - contains string value, means the state of current session.
	* **playing** - game is being played now.
	* **finished** - game is finished. You still can get info about passed game. A day later info is being cleared.
# Example with state: playing
* **players** - all of the players in the match. There is their position and login
* **terrain** - all of the obstacles in the match. There is their position and type. For now type can only be the `wall`, means that players cannot pass through it. 
* **now_turn** - login of the player who is moving now.
* **objects** - all of the objects in the match. This objects are building the core gameplay, like bombs, guns, bullets etc. Each object has it's unique id and type. Type can be: `bomb`, `bullet`, `gun` etc. Each type share basic parameters as well as their own parameters. **common parameters**:
	* **position** - position of the object.
	* **type** - string defines which type of object is it.
	* **owner** - login of the player who created this object.
	* **actor_id** - unique ID of each actor. Actor is an object or player which can interact with game field. 1 and 0 ID values are defined for both players.
	
	**unique parameters**:
	* **bomb**:
		* *ticks_left* - how many player moves remained to explode 
```json
{
    "map_size": {
        "height": 8,
        "width": 8
    },
    "move_number": 3,
    "now_turn": "2345234",
    "objects": [
        {
            "actor_id": 10,
            "direction": "down",
            "owner": "Пум",
            "position": {
                "x": 5,
                "y": 1
            },
            "shots_left": 2,
            "ticks_to_shot": 3,
            "type": "gun"
        },
        {
            "actor_id": 12,
            "owner": "Пум",
            "position": {
                "x": 2,
                "y": 3
            },
            "ticks_left": 1,
            "type": "bomb"
        },
        {
            "actor_id": 13,
            "direction": "down",
            "owner": "Пум",
            "position": {
                "x": 5,
                "y": 2
            },
            "type": "bullet"
        }
    ],
    "players": [
        {
            "actor_id": 0,
            "login": "Пум",
            "position": {
                "x": 4,
                "y": 1
            }
        },
        {
            "actor_id": 1,
            "login": "2345234",
            "position": {
                "x": 3,
                "y": 6
            }
        }
    ],
    "state": "playing",
    "terrain": [
        {
            "actor_id": 2,
            "position": {
                "x": 0,
                "y": 2
            },
            "type": "wall"
        },
        {
            "actor_id": 3,
            "position": {
                "x": 3,
                "y": 2
            },
            "type": "wall"
        }
    ]
}
```
# Example with state: finished
* **player 1/player 2** - logins of the players who participated in the current session.
* **winner** - number `1` or `2`. 1 - first player won, `2` - second player won.
```json
{
	"state": "finished",
	"winner": "login1",
	"loser": "login2"
}
```