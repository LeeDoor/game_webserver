// makes given cell a wall
function makeWall(cell){
    cell.type = "wall";
}

// updates all obstacle objects by server's data
function updateTerrain(terrain){
    for(i = 0; i < terrain.length; ++i){
        let block = terrain[i];
        makeWall(grid[block.posX][block.posY]);
    }
}

// updates all player objects by server's data
function updatePlayers(players){
    let dataUs, dataEnemy;
    if (players[0].login == localStorage.getItem('login')){
        dataUs = players[0];
        dataEnemy = players[1];
    }
    else{
        dataUs = players[1];
        dataEnemy = players[0];
    }
    playerUs.x = dataUs.posX;
    playerUs.y = dataUs.posY;

    playerEnemy.x = dataEnemy.posX;
    playerEnemy.y = dataEnemy.posY;
}



function updateObjects(objects_){
    objects = [];
    for (obj of objects_){
        objects.push(new Object(obj.posX, obj.posY, obj.type));
    }
}

// updates all objects on scene
function updateScene(){
    const terrain = lastSessionState.terrain;
    const players = lastSessionState.players;
    const objects = lastSessionState.objects;
    updateTerrain(terrain);
    updatePlayers(players);
    updateObjects(objects);
    gridSize == lastSessionState.map_size.width ? context.clearRect(0, 0, canvas.width, canvas.height) : null;
    now_turn = lastSessionState.now_turn == localStorage.getItem('login');
    drawScene();
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
    updateScene();
} 