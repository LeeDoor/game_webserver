![alt text](docs/media/header.png)

# DINOZAVRIKI
meet the best turn-based strategy about Dinosaurs battle! In this game, you face a difficult task: defeat the enemy using bombs, guns, whiskers, paws and tail.
## how to play?
click on the link and go to the game page. enter your username and password in the registration field. After the message about the successful registration, click **play**! you will end up in the waiting queue of the opponent.
## gameplay 
On the battlefield, you can use different things that will help you win. select the action on the button at the bottom. 
You will see blue cells that you can use an action on. for example, you can only walk up, down, left and right, and put a bomb anywhere around you. You can't walk on the stones, you can't put bombs on them either((.

Walk across the field and look around by selecting the walk button and clicking on the blue cell next to it!
> if there is no blue cells near you, wait for opponent's move!

![there should be a video...](docs/media/walk_video.gif) 

Now, when you get close to the enemy, drop a bomb on his head!

![there should be a video...](docs/media/bomb_video.gif)

and that is how to win on the battlefield! good luck! 
# application
if you want to launch this webserver on your machine, follow [docker](docs/docker.md) guide. 

# documentation
NOTE: I will try to change and supplement the structure of the code immediately along with the documentation.
as i write my code i also try to documentate it. here is the list of readmes for each module:
* [new module creation](docs/new_module_creation.md)
* [initializer](docs/initializer.md)
* [server](docs/server.md)
* [http_handler](docs/http_handler.md)
* [serializer(JSON)](docs/serializer.md)
* [user_manager](docs/user_manager.md)
* [token_manager](docs/token_manager.md)
* [queue_manager](queue_manager.md)
* [game_manager](docs/game_manager.md)
* [session_manager](docs/session_manager.md)
* [notification_system](docs/notification_system.md)
* [API](docs/http_api.md)
* [CLI parameters to executable](docs/CLI%20parameters%20to%20executable.md)
* [session state](docs/session_state.md)
* [SQL table init](docs/database%20sql%20tables.md)
how to write tests, naming rules etc. [here](docs/tests.md)