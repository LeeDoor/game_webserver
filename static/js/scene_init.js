// makes given cell a wall
function makeWall(cell){
    cell.type = "wall";
}

// updates all obstacle objects by server's data
function initTerrain(terrain){
    for(i = 0; i < terrain.length; ++i){
        let block = terrain[i];
        makeWall(grid[block.posX][block.posY]);
    }
}

// updates all player objects by server's data
function initPlayers(players){
    let dataUs, dataEnemy;
    if (players[0].login == login){
        dataUs = players[0];
        dataEnemy = players[1];
    }
    else{
        dataUs = players[1];
        dataEnemy = players[0];
    }
    playerUs.posX = dataUs.posX;
    playerUs.posY = dataUs.posY;
    playerUs.actor_id = dataUs.id;

    playerEnemy.posX = dataEnemy.posX;
    playerEnemy.posY = dataEnemy.posY;
    playerEnemy.actor_id = dataEnemy.id;
}



function initObjects(objects_){
    objects = [];
    for (obj of objects_){
        placeBombPlayer(obj.posX, obj.posY, obj.id, obj.owner, obj.ticks_left);
    }
}

// updates all objects on scene
function initScene(){
    const terrain = lastSessionState.terrain;
    const players = lastSessionState.players;
    const objects = lastSessionState.objects;
    initTerrain(terrain);
    initPlayers(players);
    initObjects(objects);
    gridSize == lastSessionState.map_size.width ? context.clearRect(0, 0, canvas.width, canvas.height) : null;
    now_turn = lastSessionState.now_turn == login;
    last_move = lastSessionState.move_number + 1;
}
// loads and updates information from the server immediately
async function loadScene(){
    const stateResponse = await 
        fetch('http://' + IPADDR + '/api/game/session_state?sessionId='+sessionId, {
            method: 'GET',
            headers: {
                'Content-Type': 'application/json',
                'Authorization':'Bearer ' + localStorage.getItem('token')
            },
        });
    lastSessionState = await stateResponse.json();
    initScene();
} 