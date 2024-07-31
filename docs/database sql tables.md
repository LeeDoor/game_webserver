# users
```SQL
CREATE TABLE users (
	id UUID PRIMARY KEY,
	login VARCHAR(30) UNIQUE NOT NULL,
	password VARCHAR(30) NOT NULL
);
```
# sessions
```SQL
CREATE TABLE sessions (
	id char(16) PRIMARY KEY,
	winner UUID,
	loser UUID,
	FOREIGN KEY (winner) 
		REFERENCES users(id) 
		ON DELETE SET NULL,
	FOREIGN KEY (loser)
		REFERENCES users(id) 
		ON DELETE SET NULL
);
```
