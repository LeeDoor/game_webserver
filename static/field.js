
IPADDR = 'localhost:8080';
class Cell{
    constructor(x,y,type, selected){
        this.x = x;
        this.y = y;
        this.type = type;
        this.selected = selected;
    }
}

class Player{
    constructor(x,y,us){
        this.x = x;
        this.y = y;
        this.us = us;
    }
}

class Element{
    constructor(w,h,t,l){
        this.w = w;
        this.h = h;
        this.t = t;
        this.l = l;
    }
}

class Object {
    constructor(x, y, type){
        this.x = x;
        this.y = y;
        this.type = type;
    }
}

const urlParams = new URLSearchParams(window.location.search);
const sessionId = urlParams.get('sessionId');

const canvas = document.getElementById('canvas'); // canvas
const ctx = canvas.getContext('2d'); // canvas context

let gridSize = 8; // size of grid in cells (grid is square)
const innerCellMarginps = 0.5; // canvas size's persents% between cells
let cellMarginpx; // current size between cells in pixels
let grid = []; // two-dimentional array of cells

let lastSessionState; // last state response from server

let playerUs = new Player(0,0, true); // our player object
let playerEnemy = new Player(0,0, false); // enemy player object

let selectedCell; // highlighted celected cell

let now_turn = true;
let validCells = [];

let objects = [];

// creates cells for grid
function initGrid(){
    grid = [];
    for(x = 0; x < gridSize; ++x){
        let col = [];
        for(y = 0; y < gridSize; ++y){
            col.push(new Cell(x,y,"grass", false));
        }
        grid.push(col);
    }
}

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
    const element = elementData(cell.x, cell.y);
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
            ctx.fillStyle = "rgb(191 255 252)";
            break;
    }
    ctx.fillRect(element.l, element.t, element.w, element.h);
}

// draws player
function drawPlayer(player){
    const element = elementData(player.x, player.y);
    
    if(player.us)
        ctx.fillStyle = "rgb(66 79 255)";
    else
        ctx.fillStyle = "rgb(255 145 145)";
    
    ctx.fillRect(element.l, element.t, element.w, element.h);
}

// draws all players
function drawPlayers(){
    drawPlayer(playerUs);
    drawPlayer(playerEnemy);
}

function ValidCell(cell){
    return cell && cell.x >= 0 && cell.x < gridSize &&
        cell.y >= 0 && cell.y < gridSize && cell.type == "grass"; 
}
function DefineWalkValidCell() {
    validCells = [];
    if(!now_turn) return;
    let x = playerUs.x, y = playerUs.y;
    if(ValidCell(grid[x - 1][y])) validCells.push(grid[x - 1][y]);
    if(ValidCell(grid[x + 1][y])) validCells.push(grid[x + 1][y]);
    if(ValidCell(grid[x][y - 1])) validCells.push(grid[x][y - 1]);
    if(ValidCell(grid[x][y + 1])) validCells.push(grid[x][y + 1]);
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
        const element = elementData(obj.x, obj.y);
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

// draws all objects on scene
function drawScene(){
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    drawGrid();
    drawPlayers();
    drawObjects();
}

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
    DefineWalkValidCell();
    drawScene();
}
     
// event for resizing canvas to keep with normal size
async function resizeCanvas() {
    canvas.width = parseFloat(getComputedStyle(canvas).width);
    canvas.height = parseFloat(getComputedStyle(canvas).height);

    drawScene();
}

// returns mouse position in the canvas
function getMousePos(canvas, event) {
    var rect = canvas.getBoundingClientRect();
    return {
        x: event.clientX - rect.left,
        y: event.clientY - rect.top,
    };
}

// returns cell which is mouse hovered on
function getCell(coordinates) {
    const x = Math.floor(coordinates.x/(canvas.width/gridSize));
    const y = Math.floor(coordinates.y/(canvas.height/gridSize));
    const fit = x >= 0 && x < gridSize && y >= 0 && y < gridSize;
    if(fit)
        return grid[x][y];
    return null;
}

// click event
function onClick(e) {

    if ("which" in e)  // Gecko (Firefox), WebKit (Safari/Chrome) & Opera
        isRightMB = e.which == 3; 
    else if ("button" in e)  // IE, Opera 
        isRightMB = e.button == 2; 
    if(isRightMB){
        if(walk()){
            playerUs.x = selectedCell.x;
            playerUs.y = selectedCell.y;
            drawScene();
        }
    }
    else{
        if(placeBomb()){
            drawScene();
        }
    }
}

// drops selected cell to null
function dropSelectedCell(){
    if(selectedCell){
        selectedCell.selected = false;
        selectedCell = null;
    }
}

// mouse hovering event
function onMouseOver(event){
    dropSelectedCell();
    const mouse = getMousePos(canvas, event);
    const cell = getCell(mouse);
    if (!cell) return;
    cell.selected = true;
    selectedCell = cell;
    drawScene();
}

// mouse out event
function onMouseOut(event){
    dropSelectedCell();
    drawScene();
}

// asyncronically waits for opponent move
function waitForStateChange() {
    fetch('http://' + IPADDR + '/api/game/session_state_change?sessionId='+sessionId, 
    {
        method: 'GET',
        headers: {
            'Content-Type': 'application/json',
            'Authorization':'Bearer ' + localStorage.getItem('token')
        },
    }).then(response=>{
        if (!response.ok)
            throw "response in non-ok";
        return response.json();
    }).then(json=>{
        lastSessionState = json;
        if(json.state == "finished")
            window.location.href = 'http://' + IPADDR + '/session_state.html?sessionId='+sessionId;
        else{
            waitForStateChange();
            updateScene();
        }
    });
}

function walk(){
    const data = {
        move_type: "walk",
        posX: selectedCell.x,
        posY: selectedCell.y
    };
    if(!now_turn)
        return false;
    if(Math.abs(data.posX - playerUs.x) + Math.abs(data.posY - playerUs.y) != 1)
        return false;

    move(data);
    return true;
}

function placeBomb(){
    const data = {
        move_type: "place_bomb",
        posX: selectedCell.x,
        posY: selectedCell.y
    };
    if(!now_turn)
        return false;
    if( Math.abs(data.posX - playerUs.x) > 1 &&
        Math.abs(data.posY - playerUs.y) > 1)
        return false;

    move(data);
    return true;
}

// sends player's moving
function move(data){
    fetch('http://' + IPADDR + '/api/game/move?sessionId='+sessionId, 
    {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Authorization':'Bearer ' + localStorage.getItem('token')
        },
        body: JSON.stringify(data)
    }).then(response=>{
        updateScene();
        return response.json();
    }).then(json=>{
        console.log(json);
    });
}


function resign(){
    fetch('http://' + IPADDR + '/api/game/resign?sessionId='+sessionId, 
    {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Authorization':'Bearer ' + localStorage.getItem('token')
        },
    });
}

initGrid();
loadScene();
resizeCanvas();
waitForStateChange();

window.addEventListener('resize', resizeCanvas, false);
canvas.addEventListener('click', onClick, false);
canvas.addEventListener('mousemove', onMouseOver, false);
canvas.addEventListener('mouseout', onMouseOut, false);
document.getElementById('resign-button').addEventListener('click', resign);