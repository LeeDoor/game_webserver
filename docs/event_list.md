# List of events
list of events contains an array of events. Each event share common properties. List is being filled with other events after each player's move. Subscribe to `session_state_change` to get event_list as soon as it changes.
**Fields**:
* **actor_id** - id of actor who did the action. It can be player's id or object's id. 
* **data** - data is optional and has different values depend on **event_type**.
	* PlaceData - position X and Y of the place where action happened.
	* DirectedPlaceData - position X and Y and direction where action happened. Direction is enum `"up", "right", "down", "left"`.
* **event_type** - type of the event. Defines what actually happened in the game. 
	* player_walk - player walks to a position. Data contains `PlaceData`.
	* player_place_bomb - player places bomb on provided `PlaceData`.
	* player_resign - player resigns. No data.
	* player_won - player wins the game. No data.
	* bomb_ticking - bomb ticks and getting ready to explode. No data.
	* bomb_explode - bomb explodes and kills everyone nearby 1 cell. No data.
	* #TODO

* **move_number** - number of the move, starts with 1 and increments by 1 each player's move.
> The information is written manually for a visual example of each type of event.
```json
[
  {
    "event_type": "player_place_bomb",
    "actor_id": 0,
    "data": {
      "new_object": {
        "actor_id": 2
      },
      "place": {
        "X": 1,
        "Y": 0
      },
      "ticks_left": 3
    },
    "move_number": 1
  },
  {
    "event_type": "bomb_ticking",
    "actor_id": 2,
    "data": null,
    "move_number": 1
  },
  {
    "event_type": "player_walk",
    "actor_id": 1,
    "data": {
      "place": {
        "X": 2,
        "Y": 2
      }
    },
    "move_number": 2
  },
  {
    "event_type": "bomb_explode",
    "actor_id": 2,
    "data": null,
    "move_number": 3
  },
  {
    "event_type": "player_resign",
    "actor_id": 0,
    "data": null,
    "move_number": 4
  },
  {
    "event_type": "player_won",
    "actor_id": 1,
    "data": null,
    "move_number": 4
  }
]
```