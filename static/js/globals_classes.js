class Cell{
    constructor(position,type, selected){
        this.position = position;
        this.type = type;
        this.selected = selected;
    }
}

class Player{
    constructor(position,us, actor_id){
        this.position = position;
        this.us = us;
        this.actor_id = actor_id;
        this.state = "idle";
        this.dir = "left";
        this.style = us? "green_dino" : "red_dino";
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
    constructor(position, actor_id, parent_aid){
        this.position = position;
        this.type = "object";
        this.actor_id = actor_id;
        this.parent_aid = parent_aid;
    }
}
class Bomb {
    constructor(position, actor_id, parent_aid, ticks_left){
        this.position = position;
        this.type = "bomb";
        this.actor_id = actor_id;
        this.parent_aid = parent_aid;
        this.ticks_left = ticks_left;
    }
}
class Gun {
    constructor(position, dir, actor_id, parent_aid, shots, cooldown){
        this.position = position;
        this.dir = dir;
        this.type = "gun";
        this.actor_id = actor_id;
        this.parent_aid = parent_aid;
        this.shots = shots;
        this.cooldown = cooldown;
        this.state = 0;
    }
}
class Bullet {
    constructor(position, dir, actor_id, parent_aid){
        this.position = position;
        this.dir = dir;
        this.type = "bullet";
        this.actor_id = actor_id;
        this.parent_aid = parent_aid;
    }
}
class Effect{
    constructor(type, cell){
        this.type = type;
        this.cell=cell;
    }
}
let current_move_action = "walk";

const urlParams = new URLSearchParams(window.location.search);
const sessionId = urlParams.get('sessionId');
const login = localStorage.getItem('login');

let TICKS_LEFT = 2;
let BOMB_RADIUS = 2;
let SHOTS_REMAINING = 5;
let SHOT_COOLDOWN = 2;
let GUN_PLACE_RADIUS;
let BOMB_PLACE_RADIUS;

const canvas = document.getElementById('canvas'); // canvas
const ctx = canvas.getContext('2d'); // canvas context

let gridSize = 8; // size of grid in cells (grid is square)
const innerCellMarginps = 0.5; // canvas size's persents% between cells
let cellMarginpx; // current size between cells in pixels
let grid = []; // two-dimentional array of cells

let lastSessionState; // last state response from server

let playerUs = new Player(0,0, true, 0); // our player object
let playerEnemy = new Player(0,0, false, 0); // enemy player object

let selectedCell; // highlighted celected cell
let selectedDirectionCell;

let now_turn = true;
let validCells = [];

let objects = [];
let last_move = 0;

let effects = [];