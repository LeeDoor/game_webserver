# Basics
Dinozavriki is a Turn-Based strategy. Two players are fighting as dinosaurs and trying to kill each other using handy objects like bombs, guns and even terrain objects. The game takes place on a 10 by 10 field, filled with rocks. One of the players is assigned as First and should move first. There are several possible moves to choose from: **Walk**, **Place Bomb**, **Place Gun** or **Resign**. The game finishes when one of the player resigns, explodes on the bomb or gets shot at. Now let's look at each move option in more detail.

# Game Mechanics
## Walk
![there should be a video...](docs/media/walk_video.gif) 
Player can walk on nearby cells. nearby cells are cell above, below, to the right and to the left of the player, as displayed on the gif above. To walk, select the *red walk button* below the field and click on one of available cells. Player can't walk on the rocks, game objects and other players. After the first player makes a move, the move goes to the next player.
## Place Bomb
![there should be a video...](docs/media/bomb_video.gif)
Player can place a bomb on some surrounding cell. bombs placing radius, ticks to the explosion and explosion radius are always changing #TODO, but for now: you can place bomb on any **free** cell (**free** means that this cell does not contain rocks, players and objects) in **square 5x5** centered on the player. To explode, bomb needs **two** **`ticks`**. 

> One **`tick`** happens when one of the players passes his move to other player. 

Explosion radius is **square 3x3** centered on the bomb. any player in explosion radius accounts as dead and this player loses the game. If there are two players in explosion radius at the same time, winner is **randomly assigned**.
## Place Gun
![there should be a video...](docs/media/gun_video.gif)
Player can place a gun on some surrounding cell. as mentioned above, #TODO all game rules are being changed at the moment. For now, gun can be placed in any near cell including diagonal. After selecting gun position, player should select gun direction: Up, Right, Down or Left, clicking on gun's adjacent cell.
Gun is shooting for each 3 **ticks** with a special bullet. After 3 bullets have been fired, gun breaks and disappears from the map. Each bullet moves 1 cell per tick in gun's direction and destroys on collision with rock, player or other object. If bullet hits the player, this player is dead.
## Resign
Player can press Resign button to lose immediately. simple as that.

