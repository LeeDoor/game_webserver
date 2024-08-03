# what is it for
notification system notifies users about in-game actions e.g. player found, enemy made a move etc.
it is achieved by using long-poll requests. user sends request to server and waits for response. once server is ready to respond, it sends response to active poll.
all notifiers are Singleton objects for easier access.
## why singletons?
other managers which handlers and other dudes use are just shared_pointers to one object. but notifiers cant be like that, because they store data about sockets, where we should call long-poll response. it cant be stored in third-party db, so singleton/global variable required.
# classes
* **QueueNotifier** - class, which takes care about notifying in-queue players about found enemies. when opponent is found, player's hanging poll returns sessionId. if opponent found before poll request sent, you still able to get response by waiting system. once notifier calls, all pollers are being unsubscribed. 
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
* **SessionStateNotifier** - class, which takes care about notifying players in the game and game spectators about happening actions in the game. once notified, notifier waits for a new poll and resubs user untill it unsubscribes. it means if session state updated since last poll and new poll was not sent yet, a new poll will get response immediately. if a game state updated twice or more, poll will get last game state.
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
	
	class GameStateNotifier{
		+Subscribe(uuid, sessionid)
		+Unsubscribe(uuid, sessionid)
		+Notify(sessionid)
		+ChangePoll(uuid, sessionid, poll)
		
		-GetGameState(sessionid) state
		-sessions_: map~sessionid, sessiondata~
	}
	note for SesssionData "for each sessionId there is 
	one sessionData object. it contains vector of users,
	 who didnt send poll yet, but they should get 
	 notification. also it contains map, which defines 
	 poll responser for each uuid. if poll is responded 
	 and not updated, its nullopt"
	class SessionData{
		users_responser: map~uuid,responser_opt~
		poll_waiting: vector~uuid~
	}
	GameStateNotifier-->SessionData
	
	class GameManager{
	}
	GameManager-->GameStateNotifier
	GameManager-->QueueNotifier
```