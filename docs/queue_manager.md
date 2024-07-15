# [queue_manager](https://github.com/LeeDoor/hex_chess_backend/tree/main/src/queue_manager)
## what is it for
this module communicates with redis to contain queue of players to join match, determines the pairs of players to play together and passes players to the next module [[game_manager]] to create game.
##  classes
* **IQueueManager** - interface provides all options that database communicator should have. using this class, [api handler](http_handler.md) and matchmaking_balancer interacts with the queue.
* **QueueManagerRedis** - child of **IQueueManager**. provides realization for that interface for redis database.
* **MatchmakingBalancer** - class for balancing players in queue and creating matches for them.
## graph
```mermaid
---
title: matchmaking_system classes
---
classDiagram
	class IQueueManager {
		EnqueuePlayer(uuid) bool
		DequeuePlayer(uuid) bool
		PopPlayer() string
		GetLength() long long
		GetQueue(start, end) Uuids
	}
	class QueueManagerRedis{
	}
	class MatchmakingBalancer{
		Ballance()
	}
	QueueManagerRedis --|> IQueueManager
	MatchmakingBalancer --> IQueueManager

```