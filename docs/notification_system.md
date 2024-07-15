# what is it for
module contains some notificadors for some purposes. all notifiers are Singletons. #TODO fix if i changed my mind
# notificator lists
## NetworkNotifier
NetworkNotifier contains link between player's uuid and his tcp::stream. it means that when the game needs to notify player about some action (opponent's move, game found etc.), game sends this notification by that class. streams must be subscribed to that notifier with SessionFunction #TODO change if needed.