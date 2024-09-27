function walkPlayer(player, position){
    player.position = position;
}

function placeBombPlayer(position, actor_id, parent_aid, ticks_left){
    objects.push(new Bomb(position, actor_id, parent_aid, ticks_left));
}
function placeGunPlayer(position, dir, actor_id, parent_aid, shots, cooldown){
    objects.push(new Gun(position, dir, actor_id, parent_aid, shots, cooldown));
}
function placeBullet(position, actor_id, parent_aid){
    objects.push(new Bullet(position, actor_id, parent_aid));
}
async function gameEnded(){
    window.location.href = 'http://' + IPADDR + '/session_state.html?sessionId='+sessionId;
}

function defineDir(player, x){
    if(player.position.x == x)
        return "";
    if(player.position.x < x)
        return "right";
    return "left";
}

const TICKS_LEFT = 3;
const SHOTS_REMAINING = 3;
const SHOT_COOLDOWN = 3;

async function handleEvent(ev){
    const player = ev.actor_id == playerUs.actor_id ? playerUs : playerEnemy;
    switch(ev.event){
    case "player_walk":
        await SetStateFor(player, ["jump", "idle"], defineDir(player, ev.position.x), 3);
        walkPlayer(player, ev.position);
        await SetStateFor(player, ["jump", "idle"], "", 3);
        break;
    case "player_place_bomb":
        await SetStateFor(player, ["swing", "throw", "idle"], defineDir(player, ev.position.x), 2);
        placeBombPlayer(ev.position, ev.new_actor_id, ev.actor_id, TICKS_LEFT);
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
    case "player_place_gun":
        await SetStateFor(player, ["swing", "throw", "idle"], defineDir(player, ev.position.x), 2);
        placeGunPlayer(ev.position, ev.direction, ev.new_actor_id, ev.actor_id, SHOTS_REMAINING, SHOT_COOLDOWN);
        break;
    case "gun_waiting":
        const obj = objects.filter(obj => obj.actor_id == ev.actor_id)[0];
        --obj.cooldown;
        break;  
    case "gun_shot":{
        const obj = objects.filter(obj => obj.actor_id == ev.actor_id)[0];
        await GunShotAnimation(ev.actor_id);
        --obj.shots;
        obj.cooldown = SHOT_COOLDOWN;
        placeBullet(ev.position, ev.direction, ev.new_actor_id, ev.actor_id);
        break;
    }
    case "bullet_fly":
        await bulletFlyAnimation(ev.actor_id);
        break;
    case "gun_destroy":
    case "bullet_destroy":{
        const index = objects.map(obj => obj.actor_id).indexOf(ev.actor_id);
        if (index > -1) {
            objects.splice(index, 1); 
        }
        break;
    }
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