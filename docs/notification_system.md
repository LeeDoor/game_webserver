# what is it for
notification system notifies users about in-game actions e.g. player found, enemy made a move etc.
it is achieved by using long-poll requests. user sends request to server and waits for response. once server is ready to respond, it sends response to active poll.
all notifiers are Singleton objects for easier access.
# classes
* **QueueNotifier** - class, which takes care about notifying in-queue players about found enemies. once notifier calls, all pollers are being unsubscribed.
```mermaid
---
title: QueueNotifier purpose
---
sequenceDiagram
	actor User1
	participant Server
	actor User2
	
	User1->>Server: enqueue
	User1->>Server: wait_for_opponent
	Server-->Server: enqueuing
	User2->>Server: enqueue
	User2->>Server: wait_for_opponent
	Server-->Server: enqueuing
	Server->>User1: opponent_found
	Server->>User2: opponent_found

```
* **GameStateNotifier** - class, which takes care about notifying players in the game and game spectators about happening actions in the game. once notified, notifier waits for a new poll and resubs user untill it unsubscribes.
```mermaid
sequenceDiagram
	actor Player1
	participant Server
	actor Player2
	Player1 ->> Server: game_state_change
	Player2 ->> Server: game_state_change
	Player1 ->> Server: makes a move
	Server --> Player1: edited_game_state
	Server --> Player2: edited_game_state
	Player1 ->> Server: game_state_change
	Player2 ->> Server: game_state_change
	Player2 ->> Server: makes a move
	Server --> Player1: edited_game_state
	Server --> Player2: edited_game_state
	
```

## graph
```mermaid
---
title: notification_system classes
---
classDiagram
	class QueueNotifier-PollData{
		additional_data : string
		code: status_code Ok,PollClosed
	}
	class QueueNotifier{
		+Subscribe(uuid, responser)
		+Unsubscribe(uuid, reason)
		+Notify(uuid, poll_data)

		-requests: map uuid->responser
		-wait_for_poll: uuid->poll_data
	}
	QueueNotifier--> QueueNotifier-PollData
	
	class GameStateNotifier-PollData{
		session_id : string
	}
	class GameStateNotifier{
		+Subscribe(uuid, sessionid, responser)
		+Unsubscribe(uuid, sessionid, reason)
		+Unsubscribe(uuid, reason)
		+Notify(sessionid, poll_data)

		-requests: map sessionid->uuid, responser
		-wait_for_poll: sessionid->poll_data
	}
	GameStateNotifier--> GameStateNotifier-PollData

	class GameManager{
	}
	GameManager-->GameStateNotifier
	GameManager-->QueueNotifier
```