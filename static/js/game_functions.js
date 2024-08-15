function walkPlayer(player, x, y){
    player.posX = x;
    player.posY = y;
}

function placeBombPlayer(x, y, actor_id, parent_aid, ticks_left){
    objects.push(new Bomb(x, y, actor_id, parent_aid, ticks_left));
}

async function gameEnded(actor_id){
    if(actor_id == playerUs.actor_id)
        alert("YOU WON!");
    else
        alert("YOU LOST!");
    await new Promise(r => setTimeout(r, 2000));
    window.location.href = 'http://' + IPADDR + '/session_state.html?sessionId='+sessionId;
}

async function handleEvent(ev){
    switch(ev.event_type){
    case "player_walk":
        let player = ev.actor_id == playerUs.actor_id ? playerUs : playerEnemy;
        walkPlayer(player, ev.data.place.posX, ev.data.place.posY);
        break;
    case "player_place_bomb":
        placeBombPlayer(ev.data.place.posX, ev.data.place.posY, ev.data.new_object.actor_id, ev.actor_id, ev.data.ticks_left);
        break;
    case "player_won":
        await gameEnded(ev.actor_id);
        break;
    case "bomb_ticking":
        await highlightActor(ev.actor_id);
        break;
    case "bomb_explode":
        await highlightActor(ev.actor_id, 5);
        const index = objects.map(obj => obj.actor_id).indexOf(ev.actor_id);
        console.log("index of exploded: " + index);
        if (index > -1) {
            objects.splice(index, 1); 
        }
        break;
    }
}

async function handleEvents(events){
    enemy_move = true;
    for(ev of events){
        await handleEvent(ev);
    }
    last_move = events[events.length - 1].move_number + 1;

    now_turn = !now_turn;
    drawScene();
}