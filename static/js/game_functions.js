function walkPlayer(player, x, y){
    player.X = x;
    player.Y = y;
}

function placeBombPlayer(x, y, actor_id, parent_aid, ticks_left){
    objects.push(new Bomb(x, y, actor_id, parent_aid, ticks_left));
}

async function gameEnded(){
    window.location.href = 'http://' + IPADDR + '/session_state.html?sessionId='+sessionId;
}

function defineDir(player, x){
    if(player.X == x)
        return "";
    if(player.X < x)
        return "right";
    return "left";
}

async function handleEvent(ev){
    const player = ev.actor_id == playerUs.actor_id ? playerUs : playerEnemy;
    switch(ev.event_type){
    case "player_walk":
        await SetStateFor(player, ["jump", "idle"], defineDir(player, ev.data.place.X), 3);
        walkPlayer(player, ev.data.place.X, ev.data.place.Y);
        await SetStateFor(player, ["jump", "idle"], "", 3);
        break;
    case "player_place_bomb":
        await SetStateFor(player, ["swing", "throw", "idle"], defineDir(player, ev.data.place.X), 2);
        placeBombPlayer(ev.data.place.X, ev.data.place.Y, ev.data.new_object.actor_id, ev.actor_id, ev.data.ticks_left);
        break;
    case "player_won":
        await gameEnded();
        break;
    case "bomb_ticking":
        await tickBomb(ev.actor_id);
        break;
    case "bomb_explode":
        explodeAnimation(ev.actor_id);
        await tickBomb(ev.actor_id);
        const index = objects.map(obj => obj.actor_id).indexOf(ev.actor_id);
        if (index > -1) {
            objects.splice(index, 1); 
        }
        break;
    }
}

async function handleEvents(events){
    const next_turn = !now_turn;
    now_turn = false;
    enemy_move = true;
    for(ev of events){
        await handleEvent(ev);
    }
    last_move = events[events.length - 1].move_number + 1;

    now_turn = next_turn;
}