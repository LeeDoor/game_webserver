
const gridElement = document.getElementById('grid');
const fieldElement = document.getElementById('players-field');
const gridSize = 15;
let field =[];
let playerCell;
let enemyCell;
let player, enemy;

function createGrid(){
    for (let y = 0; y < gridSize; ++y) {
        for(let x = 0; x < gridSize; ++x){
            const cell = document.createElement('div');
            cell.classList.add('cell');
            cell.position_x = x;
            cell.position_y = y;
            cell.grid_index = y*gridSize + x;
            cell.classList.add('grass');
            gridElement.appendChild(cell);
            field.push(cell);
        }
    }
} 

function makeWall(cell){
    cell.classList.remove('grass');
    cell.classList.add('wall');
}

function updatePosition(){
    player.style.top = playerCell.position_y/gridSize*100 + "%";
    player.style.left = playerCell.position_x /gridSize*100 + "%";

    enemy.style.top = enemyCell.position_y/gridSize*100 + "%";
    enemy.style.left = enemyCell.position_x /gridSize*100 + "%";
}

async function applyState(){
    const state_response = await 
        fetch('http://localhost:9999/api/game/session_state?sessionId='+localStorage.getItem('sessionId'), {
            method: 'GET',
            headers: {
                'Content-Type': 'application/json',
                'Authorization':'Bearer ' + localStorage.getItem('token')
            },
        });
    const json = await state_response.json();
    const terrain = json.terrain;
    for(i = 0; i < terrain.length; ++i){
        let block = terrain[i];
        makeWall(field[block.posY*gridSize + block.posX]);
    }

    let us, enemy;
    if (json.players[0].login == localStorage.getItem('login')){
        us = json.players[0];
        enemy = json.players[1];
    }
    else{
        us = json.players[1];
        enemy = json.players[0];
    }
    playerCell.position_x = us.posX;
    playerCell.position_y = us.posY;

    enemyCell.position_x = enemy.posX;
    enemyCell.position_y = enemy.posY;

    updatePosition();
}

async function initField() {
    createGrid();
    // Place player on a random grass cell
    const grassCells = field.filter(cell => cell.classList.contains('grass'));
    const randomIndex = Math.floor(Math.random() * grassCells.length);

    playerCell = grassCells[randomIndex];
    player = document.createElement('div');
    player.classList.add('player');
    fieldElement.appendChild(player);


    enemyCell = grassCells[randomIndex > 0? randomIndex-1 :randomIndex+1];
    enemy = document.createElement('div');
    enemy.classList.add('enemy');
    fieldElement.appendChild(enemy);

    applyState();
    updatePosition();
}
function movePlayer(direction) {
    let newPlayerCell;
    const currentIndex = playerCell.grid_index;
    const row = playerCell.position_y;
    const col = playerCell.position_x;

    switch (direction) {
        case 'up':
            if (row > 0) newPlayerCell = gridElement.children[currentIndex - gridSize];
            break;
        case 'down':
            if (row < gridSize - 1) newPlayerCell = gridElement.children[currentIndex + gridSize];
            break;
        case 'left':
            if (col > 0) newPlayerCell = gridElement.children[currentIndex - 1];
            break;
        case 'right':
            if (col < gridSize - 1) newPlayerCell = gridElement.children[currentIndex + 1];
            break;
    }
    
    function Visitable(cell){
        console.log(cell.position_x, cell.position_y);
        return cell.classList.contains('grass') && cell != enemyCell;
    }

    if (newPlayerCell && Visitable(newPlayerCell)) {
        player.style.top = newPlayerCell.position_y/gridSize*100 + "%";
        player.style.left = newPlayerCell.position_x/gridSize*100 + "%";
        playerCell = newPlayerCell;
    }
}

document.addEventListener('keydown', (event) => {
    switch (event.key) {
        case 'ArrowUp':
        case 'w':
            movePlayer('up');
            break;
        case 'ArrowDown':
        case 's':
            movePlayer('down');
            break;
        case 'ArrowLeft':
        case 'a':
            movePlayer('left');
            break;
        case 'ArrowRight':
        case 'd':
            movePlayer('right');
            break;
    }
});
initField();