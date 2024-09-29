# List of events
list of events contains an array of events. Each event share common properties. List is being filled with other events after each player's move. Subscribe to `session_state_change` to get event_list as soon as it changes.
**Fields**:
* **actor_id** - id of actor who did the action. It can be player's id or object's id. 
* **data** - data is optional and has different values depend on **event_type**.**
	* EmptyEvent has standard event data: actor_id, event, move_number
	* PositionEvent contains position, where event happened.
	* NewObjectEvent contains position and actor_id of new object.
	* NewDirObjectEvent contains position, direction and actor_id of new object.
	* **InteractionEvent** - event contains actor id of object, which is being interacted with, and `happened` list: list of events happened because of interaction.
* **event_type** - type of the event. Defines what actually happened in the game. 
	* player_walk - player walks to a position. Data contains `PlaceData`.
	* player_place_bomb - player places bomb on provided `PlaceData`.
	* player_resign - player resigns. No data.
	* player_won - player wins the game. No data.
	* bomb_ticking - bomb ticks and getting ready to explode. No data.
	* bomb_explode - bomb explodes and kills everyone nearby 1 cell. No data.
	* player_place_gun - player places gun on provided `PlaceData`
	* gun_waiting - gun is on cooldown and gets ready to shoot. No data.
	* gun_destroy - current gun has shot all bullets and destroyed. No data.
	* gun_shot - current gun shot a bullet on given position to given direction with provided actor_id.
	* bullet_fly - bullet flies with it's direction. Direction is not provided, it is set 1 time when bullet created. No data.
	* bullet_destroy - bullet was destroyed by smth or by wall. No data.
	* bullet_interaction - bullet collided with something and started interaction.
* **move_number** - number of the move, starts with 1 and increments by 1 each player's move.
> The information is written manually for a visual example of each type of event.
```json
[
  {
    "actor_id": 0,
    "direction": "right",
    "event": "player_place_gun",
    "move_number": 1,
    "new_actor_id": 2,
    "position": {
      "x": 0,
      "y": 0
    }
  },
  {
    "actor_id": 2,
    "event": "gun_waiting",
    "move_number": 1
  },
  {
    "actor_id": 1,
    "event": "player_walk",
    "move_number": 2,
    "position": {
      "x": 2,
      "y": 2
    }
  },
  {
    "actor_id": 2,
    "direction": "right",
    "event": "gun_shot",
    "move_number": 3,
    "new_actor_id": 3,
    "position": {
      "x": 0,
      "y": 0
    }
  },
  {
    "actor_id": 3,
    "event": "bullet_fly",
    "move_number": 3
  },
  {
    "actor_id": 3,
    "event": "bullet_destroy",
    "move_number": 5
  },
  {
    "actor_id": 1,
    "event": "player_walk",
    "move_number": 6,
    "position": {
      "x": 2,
      "y": 0
    }
  },
  {
    "actor_id": 4,
    "event": "bullet_interaction",
    "happened": [
      {
        "actor_id": 1,
        "event": "player_dead",
        "move_number": 7
      }
    ],
    "interacted_actor_id": 1,
    "move_number": 7
  },
  {
    "actor_id": 4,
    "event": "bullet_destroy",
    "move_number": 7
  },
  {
    "actor_id": 0,
    "event": "player_won",
    "move_number": 7
  }
]
```