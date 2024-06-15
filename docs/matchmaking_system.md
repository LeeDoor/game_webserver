# [matchmaking_system](https://github.com/LeeDoor/hex_chess_backend/tree/main/src/matchmaking_system)
## what is it for
this module contains queue of players to join match, determines the pairs of players to play together and passes players to the next module [[game_manager]] to create game.
##  classes
* **MMQueue** - contains a queue of players. using this class, [[http_handler|api handler]] interacts with the queue.
## actions
```mermaid
---

title: match making queue workflow

---

flowchart TB

ApiHandler --"enqueue player"-->Enqueue

ApiHandler --"dequeue player"-->Dequeue

subgraph MMQueue

Dequeue --"Update"-->QueueUpdate

Enqueue --"Update"-->QueueUpdate

end

subgraph game manager

QueueUpdate --"picks two players"--> CreateSession

end
```

## graph
```mermaid
---

title: matchmaking_system classes

---

  

classDiagram

class MMQueue {

+EnqueuePlayer(uuid) bool

+DequeuePlayer(uuid) bool

-QueueUpdate()

-queue : vector<uuid>

-game_manager : Ptr

}
```