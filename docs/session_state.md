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
	
* **events** - every event happened in the match staged here. it can be player's action, objects' action, game ending. 
```json
{
	"state": "playing",
	"players": 
	[
		{
			"id": INT,
			"login": STRING,
			"posX": UNSIGNED_INT,
			"posY": UNSIGNED_INT,
		},
		// same description for second player
	],
	"terrain":
	[
		{
			"posX": UNSIGNED_INT,
			"posY": UNSIGNED_INT,
			"type": STRING// ["wall"]
		},
		// all other walls etc.
	],
	"objects":
	[
		{
			"type": STRING, // bomb, gun etc.
			"id": INT, // unique actor id
			"posX": UNSIGNED,
			"posY": UNSIGNED,
			"owner": STRING // login of the owner
		}
	],
	"now_turn":STRING// player's login
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