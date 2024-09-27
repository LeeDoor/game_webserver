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
const EFFECTS_DIR = IMAGE_DIR + "effects/";
const players_sprites = new PlayersSprites(PLAYERS_DIR); 
const terrain_sprites = {
    grass: new Sprite(TERRAIN_DIR + "grass.svg"),
    wall: new Sprite(TERRAIN_DIR + "rock.svg"),
};
const object_sprites = {
    bomb: [
        new Sprite(OBJECTS_DIR + "bomb1.svg"),
        new Sprite(OBJECTS_DIR + "bomb2.svg"),
        new Sprite(OBJECTS_DIR + "bomb3.svg"),
        new Sprite(OBJECTS_DIR + "bomb4.svg"),
    ],
    bullet: new Sprite(OBJECTS_DIR + "bullet.svg"),
    gun: {
        left: [
            new Sprite(OBJECTS_DIR + "blue_gun/left/1.svg"),
            new Sprite(OBJECTS_DIR + "blue_gun/left/2.svg"),
            new Sprite(OBJECTS_DIR + "blue_gun/left/3.svg"),
            new Sprite(OBJECTS_DIR + "blue_gun/left/4.svg"),
        ],
        right: [
            new Sprite(OBJECTS_DIR + "blue_gun/right/1.svg"),
            new Sprite(OBJECTS_DIR + "blue_gun/right/2.svg"),
            new Sprite(OBJECTS_DIR + "blue_gun/right/3.svg"),
            new Sprite(OBJECTS_DIR + "blue_gun/right/4.svg"),
        ],
        up: [
            new Sprite(OBJECTS_DIR + "blue_gun/up/1.svg"),
            new Sprite(OBJECTS_DIR + "blue_gun/up/2.svg"),
            new Sprite(OBJECTS_DIR + "blue_gun/up/3.svg"),
            new Sprite(OBJECTS_DIR + "blue_gun/up/4.svg"),
        ],
        down: [
            new Sprite(OBJECTS_DIR + "blue_gun/down/1.svg"),
            new Sprite(OBJECTS_DIR + "blue_gun/down/2.svg"),
            new Sprite(OBJECTS_DIR + "blue_gun/down/3.svg"),
            new Sprite(OBJECTS_DIR + "blue_gun/down/4.svg"),
        ]
    }
};
const effect_sprites = {
    fire: new Sprite(EFFECTS_DIR + "fire.svg")
}

// returns cell rectangle to draw
function cellElementData(position){
    const x = position.x;
    const y = position.y;
    let res = new Element(0,0,0,0);
    const space = 0;//canvas.width/gridSize/2;
    res.w = (canvas.width - space)/gridSize;
    res.h = (canvas.height - space)/gridSize;
    res.t = y * res.h + space / 2;
    res.l = x * res.w + space / 2;

    res.t += cellMarginpx;
    res.l += cellMarginpx;
    res.w -= cellMarginpx * 2;
    res.h -= cellMarginpx * 2;

    return res;
}

function objElementData(position){
    let res = cellElementData(position);

    res.l -= res.w / 2;
    res.t -= res.h;
    res.w *= 2;
    res.h *= 2;

    return res;
}


//draws one cell
function drawCell(cell){
    const element = cellElementData(cell.position);
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
    const element = objElementData(player.position);
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
        && !(playerEnemy.position == {x:x, y:y})
        && grid[x][y].type == "grass"; 
}

function AddSideCells(position, validCells){
    const x = position.x;
    const y = position.y;
    if(ValidCell(x - 1, y - 1))validCells.push(grid[x - 1][y - 1]);
    if(ValidCell(x + 1, y - 1))validCells.push(grid[x + 1][y - 1]);
    if(ValidCell(x - 1, y + 1))validCells.push(grid[x - 1][y + 1]);
    if(ValidCell(x + 1, y + 1))validCells.push(grid[x + 1][y + 1]);
}

function AddAxialCells(position, validCells){
    const x = position.x;
    const y = position.y;
    if(ValidCell(x - 1, y))validCells.push(grid[x - 1][y]);
    if(ValidCell(x + 1, y))validCells.push(grid[x + 1][y]);
    if(ValidCell(x, y + 1))validCells.push(grid[x][y + 1]);
    if(ValidCell(x, y - 1))validCells.push(grid[x][y - 1]);
}

function DefineValidCell() {
    validCells = [];
    if(!now_turn) return;
    switch(current_move_action) {
    case "bomb":
    case "gun":
        AddSideCells(playerUs.position, validCells);
    case "walk":
        AddAxialCells(playerUs.position, validCells);
        break;
    case "gun_rotating":
        AddAxialCells(selectedCell.position, validCells);
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
        const element = objElementData(obj.position);
        let sprite;
        if(obj.type == "bomb")
            sprite = object_sprites[obj.type][3 - obj.ticks_left];
        else if(obj.type == "gun")
            sprite = object_sprites[obj.type][obj.dir][obj.state];
        else if(obj.type = "bullet")
            sprite = object_sprites[obj.type];
        else 
            sprite = object_sprites["gun"]["left"][0];
        ctx.drawImage(sprite.image, element.l, element.t, element.w, element.h);
    }
}



function drawEffects(){
    for(effect of effects){ 
        const sprite = effect_sprites[effect.type];
        const element = cellElementData(effect.cell.position);
        ctx.drawImage(sprite.image, element.l, element.t, element.w, element.h);
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
        return grid[playerUs.position.x][playerUs.position.y];
    }
    if (actor_id == playerEnemy.actor_id){
        return grid[playerEnemy.position.x][playerEnemy.position.y];
    }
    const obj = objects.filter(obj => obj.actor_id == actor_id)[0];
    return grid[obj.position.x][obj.position.x];
}

async function SetStateFor(player, states, dir, repeat){
    if(dir != "")
        player.dir = dir;
    const dur = 400/repeat/states.length;
    for(i = 0; i < repeat; ++i){
        for(let state of states){
            player.state = state;
            await new Promise(r => setTimeout(r, dur));
        }
    }
} 

async function tickBomb(actor_id) {
    const dur = 100;
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
async function GunShotAnimation(actor_id) {
    const obj = objects.filter(obj => obj.actor_id == actor_id)[0];
    
    for(i = 1; i < 4; ++i){
        obj.state = i;
        await new Promise(r => setTimeout(r, 200));
    }
    obj.state = 0;
}
async function explodeAnimation(actor_id) {
    const dur = 500;
    const repeat = 3;
    const obj = objects.filter(obj => obj.actor_id == actor_id)[0];
    let cellsToExplode = [];
    for(let x = obj.position.x - 1; x <= obj.position.x + 1; ++x){
        if (x < 0) continue;
        if (x >= gridSize) continue;
        for(let y = obj.position.y - 1; y <= obj.position.y + 1; ++y){
            if (y < 0) continue;
            if (y >= gridSize) continue;
            if (grid[x][y].type == "wall") continue;
            cellsToExplode.push(grid[x][y]);
        }
    }
    for(let i = 0; i < repeat; ++i){
        for(let cell of cellsToExplode){
            const eff = new Effect("fire", cell);
            effects.push(eff);
        }
        await new Promise(r => setTimeout(r, dur));
        for(let cell of cellsToExplode){
            const index = effects.indexOf(effects.filter(eff => eff.cell == cell));
            effects.splice(index, 1);         
        }
        await new Promise(r => setTimeout(r, dur));
    }
}
async function bulletFlyAnimation(actor_id) {
    const dur = 100;
    const obj = objects.filter(obj => obj.actor_id == actor_id)[0];
    const before = obj.position;
    let after = structuredClone(before);
    
    switch(obj.dir){
    case "up":
        after.y--;
        break;
    case "left":
        after.x--;
        break;
    case "down":
        after.y++;
        break;
    case "right":
        after.x++;
        break;
    }
    const reps = 10;
    const equal_x = before.x == after.x;
    const step = equal_x ? (after.y - before.y) / reps : (after.x - before.x) / reps;
    for(i = 0; i < reps; ++i){
        if(equal_x){
            obj.position.y += step;
        }
        else {
            obj.position.x += step;
        }
        await new Promise(r => setTimeout(r, dur/reps));
    }
}