
IPADDR = '95.220.176.106:8080';
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

let current_move_action = "walk";

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
