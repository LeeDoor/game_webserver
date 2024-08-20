class Sprite {
    constructor(imagePath) {
        this.image = new Image();
        this.image.src = imagePath;
    }
}

class PlayerSpriteGroup {
    constructor(dirPath) {
        this.idle = new Sprite(dirPath + "idle.svg");
        this.jump = new Sprite(dirPath + "jump.svg");
        this.swing = new Sprite(dirPath + "swing.svg");
        this.throw = new Sprite(dirPath + "throw.svg");
    }
}

class PlayerSprites{
    constructor(dirPath){
        this.left = new PlayerSpriteGroup(dirPath + "left/");
        this.right = new PlayerSpriteGroup(dirPath + "right/");
    }
}

class PlayersSprites{
    constructor(dirPath){
        this.green_dino = new PlayerSprites(dirPath + "green_dino/");
        this.red_dino = new PlayerSprites(dirPath + "red_dino/");
    }
}
const IMAGE_DIR = "images/";
const TERRAIN_DIR = IMAGE_DIR + "terrain/";
const OBJECTS_DIR = IMAGE_DIR + "objects/";
const PLAYERS_DIR = IMAGE_DIR + "players/";
const players_sprites = new PlayersSprites(PLAYERS_DIR); 
const terrain_sprites = {
    grass: new Sprite(TERRAIN_DIR + "grass.svg"),
    fire: new Sprite(TERRAIN_DIR + "fire.svg"),
    wall: new Sprite(TERRAIN_DIR + "rock.svg"),
};
const object_sprites = {
    bomb: [
        new Sprite(OBJECTS_DIR + "bomb1.svg"),
        new Sprite(OBJECTS_DIR + "bomb2.svg"),
        new Sprite(OBJECTS_DIR + "bomb3.svg"),
        new Sprite(OBJECTS_DIR + "bomb4.svg"),
    ]
};

// returns cell rectangle to draw
function cellElementData(x, y){
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

function objElementData(x, y){
    let res = cellElementData(x,y);

    res.l -= res.w / 2;
    res.t -= res.h;
    res.w *= 2;
    res.h *= 2;

    return res;
}


//draws one cell
function drawCell(cell){
    const element = cellElementData(cell.posX, cell.posY);
    const sprite = terrain_sprites[cell.type];
    ctx.drawImage(sprite.image, element.l, element.t, element.w, element.h);

    if(cell.selected){
        ctx.fillStyle = "rgba(51, 255, 0, 0.4)";
        ctx.fillRect(element.l, element.t, element.w, element.h);
    }

    if(validCells.includes(cell)){
        ctx.fillStyle = "rgba(0, 238, 255, 0.4)";
        ctx.fillRect(element.l, element.t, element.w, element.h);
    }
}

// draws player
function drawPlayer(player){
    const element = objElementData(player.posX, player.posY);
    const sprite = players_sprites[player.style][player.dir][player.state];
    ctx.drawImage(sprite.image, element.l, element.t, element.w, element.h);
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
        const element = objElementData(obj.posX, obj.posY);
        const sprite = object_sprites[obj.type][3 - obj.ticks_left];
        ctx.drawImage(sprite.image, element.l, element.t, element.w, element.h);
    }
}

function drawHighlighter(cell){
    const element = cellElementData(cell.posX, cell.posY);
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

async function SetStateFor(player, states, dir, repeat){
    if(dir != "")
        player.dir = dir;
    const dur = 1000/repeat/states.length;
    for(i = 0; i < repeat; ++i){
        for(let state of states){
            player.state = state;
            await new Promise(r => setTimeout(r, dur));
        }
    }
} 

async function tickBomb(actor_id) {
    const dur = 300;
    const repeat = 5;
    const obj = objects.filter(obj => obj.actor_id == actor_id)[0];
    for(let i = 0; i < repeat; ++i){
        --obj.ticks_left;
        await new Promise(r => setTimeout(r, dur));
        ++obj.ticks_left;
        await new Promise(r => setTimeout(r, dur));
    }
    --obj.ticks_left;
}