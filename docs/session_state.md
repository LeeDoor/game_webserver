# JSON
* **state** - contains string value, means the state of current session.
	* **playing** - game is being played now.
	* **finished** - game is finished. you still can get info about passed game. a day later info is being cleared.
# example with state: playing
* **players** - all of the players in the match. there is their position and login
* **terrain** - all of the obstacles in the match. there is their position and type. for now type can only be the `wall`, means that players cannot pass through it. 
* **now_turn** - login of the player who is moving now.
* **objects** - all of the objects in the match. this objects are building the core gameplay, like bombs, guns, bullets etc. each object has it's unique id and type. type can be: `bomb`, `bullet`, `gun` etc. each type share basic parameters as well as their own parameters. **common parameters**:
	* **posX** - position X of the object.
	* **posY** - position Y of the object.
	* **type** - string defines which type of object is it.
	* **owner** - login of the player who created this object.
	* **id** - unique ID of each actor. actor is an object or player which can interact with game field. 1 and 0 ID values are defined for both players.
	
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
      "posX": 1,
      "posY": 1,
      "ticks_left": 5,
      "owner": "NIGGER",
      "actor_id": 1
    }
],
  "players": [
    {
      "id": 0,
      "login": "a2668ee3a84a3429",
      "posX": 4,
      "posY": 1
    },
    {
      "id": 1,
      "login": "894fa62d011c19de",
      "posX": 3,
      "posY": 6
    }
  ],
  "state": "playing",
  "terrain": [
    {
      "posX": 0,
      "posY": 2,
      "type": "wall"
    },
    {
      "posX": 3,
      "posY": 2,
      "type": "wall"
    }
  ]
}
```
# example with state: finished
* **player1/player2** - logins of the players who participated in the current session.
* **winner** - number `1` or `2`. 1 - first player won, `2` - second player won.
```json
{
	"state": "finished",
	"winner": "login1",
	"loser": "login2"
}
```