// makes given cell a wall
function makeWall(cell){
    cell.type = "wall";
}

// updates all obstacle objects by server's data
function initTerrain(terrain){
    for(i = 0; i < terrain.length; ++i){
        let block = terrain[i];
        makeWall(grid[block.position.x][block.position.y]);
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
    playerUs = new Player(dataUs.position, true, dataUs.actor_id);
    playerEnemy = new Player(dataEnemy.position, false, dataEnemy.actor_id);
}



function initObjects(objects_){
    objects = [];
    for (obj of objects_){
        placeBombPlayer(obj.position, obj.id, obj.owner, obj.ticks_left);
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
    if (!stateResponse.ok)
        window.location.href = 'http://' + IPADDR + '/session_state.html?sessionId='+sessionId;
    lastSessionState = await stateResponse.json();
    initScene();
} 