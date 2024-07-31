# JSON
* **state** - contains string value, means the state of current session.
	* **playing** - game is being played now.
	* **finished** - game is finished. you still can get info about passed game. a day later info is being cleared.
# example with state: playing
* **players** - all of the players in the match. there is their position and login
* **terrain** - all of the obstacles in the match. there is their position and type. for now type can only be the `wall`, means that players cannot pass through it. #TODO
* **now_turn** - login of the player who is moving now.
```json
{
	"state": "playing",
	"players": 
	[
		{
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