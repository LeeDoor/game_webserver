# list of events
list of events contains an array of events. each event share common properties. list is being filled with other events after each player's move. subscribe to `session_state_change` to get event_list as soon as it changes.
**Fields**:
* **actor_id** - id of actor who did the action. it can be player's id or object's id. 
* **data** - data is optional and has different values depend on **event_type**.
	* PlaceData - position X and Y of the place where action happened.
	* DirectedPlaceData - position X and Y and direction where action happened. direction is enum `"up", "right", "down", "left"`.
* **event_type** - type of the event. defines what actually happened in the game. 
	* player_walk - player walks to a position. data contains `PlaceData`.
	* player_place_bomb - player places bomb on provided `PlaceData`.
	* player_resign - player resigns. no data.
	* player_won - player wins the game. no data.
	* bomb_ticking - bomb ticks and getting ready to explode. no data.
	* bomb_explode - bomb explodes and kills everyone nearby 1 cell. no data.
	* #TODO

* **move_number** - number of the move, starts with 1 and increments by 1 each player's move.
> the information is written manually for a visual example of each type of event.
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
        "posX": 1,
        "posY": 0
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
        "posX": 2,
        "posY": 2
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