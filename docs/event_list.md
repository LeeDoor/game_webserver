# List of events
list of events contains an array of events. Each event share common properties. List is being filled with other events after each player's move. Subscribe to `session_state_change` to get event_list as soon as it changes.
**Fields**:
* **actor_id** - id of actor who did the action. It can be player's id or object's id. 
* **additional data** - data is optional and has different values depend on **event_type**.**
	* EmptyEvent has standard event data: actor_id, event, move_number
	* PositionEvent contains position, where event happened.
	* NewObjectEvent contains position and actor_id of new object.
	* NewDirObjectEvent contains position, direction and actor_id of new object.
	* **InteractionEvent** - event contains actor id of object, which is being interacted with, and `happened` list: list of events happened because of interaction.
* **event** - type of the event. Defines what actually happened in the game. 
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
	* bullet_interaction - bullet collided with something and started **`interaction`**.
* **move_number** - number of the move, starts with 1 and increments by 1 each player's move.

> `PlaceData` is the data of place where event occured. it is called `position` and  tells the X and Y indices of the cell (0-indexed).  
```json
"position": {
  "x": 0,
  "y": 0
}
```
## Interaction event
interaction event means that two objects on the map interacted with each other, which caused some new events. Interaction event brings some new data:
* **interacted_actor_id** - actor_id of the object, with which an object interacted (actor_id here means the object which *started* interaction, i.e. if the bullet hits the wall, bullet's actor id is written to `actor_id` and the wall's actor_id is written to `interacted_actor_id`).
* **happened** - an array of events. These events are caused by the intereaction event. For example, if the bullet hits the player, this player is lost and the bullet must be destroyed. `happened` will contain the list with `bullet_destroy` and `player_dead` events.
# Example
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
    }gTgT
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