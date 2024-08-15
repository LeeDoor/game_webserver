usImg = new Image();
usImg.src = "../images/Untitled.svg";
enemyImg = new Image();
enemyImg.src = "../images/player2.png"

enemyImg.onload = ()=>{drawPlayers();};

// returns cell rectangle to draw
function elementData(x, y){
    let res = new Element(0,0,0,0);

    res.w = canvas.width/gridSize;
    res.h = canvas.height/gridSize;
    res.t = y * res.h;
    res.l = x * res.w;

    res.t += cellMarginpx;
    res.l += cellMarginpx;
    res.w -= cellMarginpx * 2;
    res.h -= cellMarginpx * 2;

    return res;
}

//draws one cell
function drawCell(cell){
    const element = elementData(cell.posX, cell.posY);
    switch(cell.type){
        case "grass":
            if (validCells.includes(cell))
                ctx.fillStyle = "rgb(135, 181, 255)";
            else if (cell.selected)
                ctx.fillStyle = "rgb(129 255 170)";
            else
                ctx.fillStyle = "rgb(79 255 170)";
            break;
        case "wall":
            ctx.fillStyle = "rgb(131 195 190)";
            break;
    }
    ctx.fillRect(element.l, element.t, element.w, element.h);
}

// draws player
function drawPlayer(player){
    const element = elementData(player.posX, player.posY);
    
    if(player.us)
        ctx.drawImage(usImg, element.l, element.t, element.w, element.h);
    else
        ctx.drawImage(enemyImg, element.l, element.t, element.w, element.h);
}

// draws all players
function drawPlayers(){
    drawPlayer(playerUs);
    drawPlayer(playerEnemy);
}

function ValidCell(x, y){
    return x >= 0 && x < gridSize &&
        y >= 0 && y < gridSize 
        && !(playerEnemy.posX == x && playerEnemy.posY == y)
        && grid[x][y].type == "grass"; 
}

function AddSideCells(x, y, validCells){
    if(ValidCell(x - 1, y - 1))validCells.push(grid[x - 1][y - 1]);
    if(ValidCell(x + 1, y - 1))validCells.push(grid[x + 1][y - 1]);
    if(ValidCell(x - 1, y + 1))validCells.push(grid[x - 1][y + 1]);
    if(ValidCell(x + 1, y + 1))validCells.push(grid[x + 1][y + 1]);
}

function AddAxialCells(x, y, validCells){
    if(ValidCell(x - 1, y))validCells.push(grid[x - 1][y]);
    if(ValidCell(x + 1, y))validCells.push(grid[x + 1][y]);
    if(ValidCell(x, y + 1))validCells.push(grid[x][y + 1]);
    if(ValidCell(x, y - 1))validCells.push(grid[x][y - 1]);
}

function DefineValidCell() {
    validCells = [];
    if(!now_turn) return;
    let x = playerUs.posX, y = playerUs.posY;
    switch(current_move_action) {
    case "bomb":
        AddSideCells(x, y, validCells);
    case "walk":
        AddAxialCells(x, y, validCells);
        break;
    }
}

// draws all cells in grid
function drawGrid(){
    cellMarginpx = canvas.width * innerCellMarginps / 100;
    for(y = 0; y < gridSize; ++y){
        for (x = 0; x < gridSize; ++x){
            drawCell(grid[x][y]);
        }
    }
}

function drawObjects(){
    for(obj of objects) {
        const element = elementData(obj.posX, obj.posY);
        switch(obj.type){
            case 'bomb':
                ctx.fillStyle = "rgb(255 255 255)";
                ctx.beginPath();
                ctx.arc(element.l + element.w/2, element.t + element.h/2, element.h/2, 0, 2 * Math.PI);
                ctx.fill();
                break;
        }
    }
}

function drawHighlighter(cell){
    const element = elementData(cell.posX, cell.posY);
    ctx.beginPath();
    ctx.strokeStyle = "red";
    ctx.lineWidth = 20;
    ctx.arc(element.l + element.w/2, element.t + element.h/2, element.h/2, 0, 2 * Math.PI);
    ctx.stroke();
}

function drawEffects(){
    for(effect of effects){
        switch(effect.type){
        case "highlighter":
            drawHighlighter(effect.cell);
            break;
        }
    }
}

// draws all objects on scene
function drawScene(){
    DefineValidCell();
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    drawGrid();
    drawPlayers();
    drawObjects();
    drawEffects();
}

function getCellByActor(actor_id){
    if (actor_id == playerUs.actor_id){
        return grid[playerUs.posX][playerUs.posY];
    }
    if (actor_id == playerEnemy.actor_id){
        return grid[playerEnemy.posX][playerEnemy.posY];
    }
    const obj = objects.filter(obj => obj.actor_id == actor_id)[0];
    return grid[obj.posX][obj.posY];
}


async function highlightActor(actor_id, repeat = 3){
    const cell = getCellByActor(actor_id);
    const delay = 1000 / repeat;
    for(i = 0; i < repeat; ++i){
        const eff = new Effect("highlighter", cell);
        effects.push(eff);
        await new Promise(r => setTimeout(r, delay));
        const index = effects.indexOf(eff);
        effects.splice(index, 1);         
        await new Promise(r => setTimeout(r, delay));
    }
}