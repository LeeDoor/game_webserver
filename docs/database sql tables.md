# users
```SQL
CREATE TABLE users (
	id UUID PRIMARY KEY,
	login VARCHAR(30) UNIQUE NOT NULL,
	password VARCHAR(30) NOT NULL
);
```
# finished_sessions
```SQL
CREATE TABLE finished_sessions (
	id UUID PRIMARY KEY,
	player1 UUID,
	player2 UUID,
	winner BIT,
	FOREIGN KEY (player1) 
		REFERENCES users(id) 
		ON DELETE SET NULL,
	FOREIGN KEY (player2)
		REFERENCES users(id) 
		ON DELETE SET NULL
);
```
