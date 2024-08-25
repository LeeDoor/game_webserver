# JSON
* **state** - contains string value, means the state of current session.
	* **playing** - game is being played now.
	* **finished** - game is finished. You still can get info about passed game. A day later info is being cleared.
# Example with state: playing
* **players** - all of the players in the match. There is their position and login
* **terrain** - all of the obstacles in the match. There is their position and type. For now type can only be the `wall`, means that players cannot pass through it. 
* **now_turn** - login of the player who is moving now.
* **objects** - all of the objects in the match. This objects are building the core gameplay, like bombs, guns, bullets etc. Each object has it's unique id and type. Type can be: `bomb`, `bullet`, `gun` etc. Each type share basic parameters as well as their own parameters. **common parameters**:
	* **X** - position X of the object.
	* **Y** - position Y of the object.
	* **type** - string defines which type of object is it.
	* **owner** - login of the player who created this object.
	* **id** - unique ID of each actor. Actor is an object or player which can interact with game field. 1 and 0 ID values are defined for both players.
	
	**unique parameters**:
	* **bomb**:
		* *ticks_left* - how many player moves remained to explode 
```json
{
  "map_size": {
    "height": 15,
    "width": 15
  },
  "move_number": 1,
  "now_turn": "a2668ee3a84a3429",
  "objects": [
    {
      "type": "bomb",
      "X": 1,
      "Y": 1,
      "ticks_left": 5,
      "owner": "NIGGER",
      "actor_id": 1
    }
],
  "players": [
    {
      "id": 0,
      "login": "a2668ee3a84a3429",
      "X": 4,
      "Y": 1
    },
    {
      "id": 1,
      "login": "894fa62d011c19de",
      "X": 3,
      "Y": 6
    }
  ],
  "state": "playing",
  "terrain": [
    {
      "X": 0,
      "Y": 2,
      "type": "wall"
    },
    {
      "X": 3,
      "Y": 2,
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