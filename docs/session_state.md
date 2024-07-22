# JSON
session state json file looks like this:
```json
{
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