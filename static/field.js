
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

const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d');

const gridSize = 15;
const borderps = 0;
const innerCellMarginps = 0.5;
let cellMarginpx;
let borderpx;
let grid = [];

let playerUs = new Player(0,0, true);
let playerEnemy = new Player(0,0, false);

let selectedCell;

function initGrid(){
    for(x = 0; x < gridSize; ++x){
        let col = [];
        for(y = 0; y < gridSize; ++y){
            col.push(new Cell(x,y,"grass", false));
        }
        grid.push(col);
    }
}

function elementData(x, y){
    let res = new Element(0,0,0,0);

    res.w = (canvas.width - 2 * borderpx)/gridSize;
    res.h = (canvas.height - 2 * borderpx)/gridSize;
    res.t = borderpx + y * res.h;
    res.l = borderpx + x * res.w;

    res.t += cellMarginpx;
    res.l += cellMarginpx;
    res.w -= cellMarginpx * 2;
    res.h -= cellMarginpx * 2;

    return res;
}


function drawCell(cell){
    const element = elementData(cell.x, cell.y);
    switch(cell.type){
        case "grass":
            ctx.fillStyle = "rgb("+(79+cell.selected*50)+" 255 170)";
            break;
        case "wall":
            ctx.fillStyle = "rgb(191 255 252)";
            break;
    }
    ctx.fillRect(element.l, element.t, element.w, element.h);
}

function drawPlayer(player){
    const element = elementData(player.x, player.y);
    if(player.us)
        ctx.fillStyle = "rgb(66 79 255)";
    else
        ctx.fillStyle = "rgb(255 145 145)";
    
    ctx.fillRect(element.l, element.t, element.w, element.h);
}

function drawPlayers(){
    drawPlayer(playerUs);
    drawPlayer(playerEnemy);
}

function drawGrid(){
    borderpx = canvas.width * borderps / 100;
    cellMarginpx = canvas.width * innerCellMarginps / 100;

    for(y = 0; y < gridSize; ++y){
        for (x = 0; x < gridSize; ++x){
            drawCell(grid[x][y]);
        }
    }
}

function drawScene(){
    drawGrid();
    drawPlayers();
}

function makeWall(cell){
    cell.type = "wall";
}

function updateTerrain(terrain){
    for(i = 0; i < terrain.length; ++i){
        let block = terrain[i];
        makeWall(grid[block.posX][block.posY]);
    }
}

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

async function updateScene(){
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
    const players = json.players;
    updateTerrain(terrain);
    updatePlayers(players);
}

        
async function resizeCanvas() {
    canvas.width = parseFloat(getComputedStyle(canvas).width);
    canvas.height = parseFloat(getComputedStyle(canvas).height);

    await updateScene();
    drawScene();
}

function getMousePos(canvas, event) {
    var rect = canvas.getBoundingClientRect();
    return {
        x: event.clientX - rect.left,
        y: event.clientY - rect.top,
    };
}
function getCell(coordinates) {
    const x = Math.floor(coordinates.x/(canvas.width/gridSize));
    const y = Math.floor(coordinates.y/(canvas.height/gridSize));
    const fit = x >= 0 && x < gridSize && y >= 0 && y < gridSize;
    console.log(x, y);
    if(fit)
        return grid[x][y];
    return null;
}


function OnClick(event) {
    playerUs.x = selectedCell.x;
    playerUs.y = selectedCell.y;
    drawScene();
}

function OnMouseOver(event){
    if(selectedCell){
        selectedCell.selected = false;
        selectedCell = null;
    }
    const mouse = getMousePos(canvas, event);
    const cell = getCell(mouse);
    if (!cell) return;
    cell.selected = true;
    selectedCell = cell;
    drawScene();
}

initGrid();
resizeCanvas();
window.addEventListener('resize', resizeCanvas, false);
canvas.addEventListener('click', OnClick, false);
canvas.addEventListener('mousemove', OnMouseOver, false);
//canvas.addEventListener('mouseout', OnMouseOut, false);
  