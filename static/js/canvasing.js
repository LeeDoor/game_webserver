// creates cells for grid
function initGrid(){
    grid = [];
    for(x = 0; x < gridSize; ++x){
        let col = [];
        for(y = 0; y < gridSize; ++y){
            col.push(new Cell({x:x,y:y},"grass", false));
        }
        grid.push(col);
    }
}

let qualMult = 3;

// event for resizing canvas to keep with normal size
async function resizeCanvas() {
    const style = getComputedStyle(canvas);
    const width = parseFloat(style.width);
    const height = parseFloat(style.height);
    if(width < 500) qualMult = 5;
    else qualMult = 2;
    canvas.width = parseFloat(width) * qualMult;
    canvas.height = parseFloat(height) * qualMult;
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
    const x = Math.floor(coordinates.x/(canvas.width / qualMult /gridSize));
    const y = Math.floor(coordinates.y/(canvas.height / qualMult/gridSize));
    const fit = x >= 0 && x < gridSize && y >= 0 && y < gridSize;
    if(fit)
        return grid[x][y];
    return null;
}

// click event
function onClick(e) {
    if(current_move_action == "walk"){
        walk();
    }
    else if (current_move_action == "bomb"){
        placeBomb();
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
}

// mouse out event
function onMouseOut(event){
    dropSelectedCell();
}



function radioButtons(){
    let buttons = document.getElementsByClassName('radio');
    for(b of buttons){
        if(current_move_action ==  b.innerText){
            b.classList.remove('unselected');
            b.classList.add("selected");
        }
        else{
            b.classList.remove('selected');
            b.classList.add('unselected');
        }
    }
}
window.addEventListener('resize', resizeCanvas, false);
canvas.addEventListener('click', onClick, false);
canvas.addEventListener('mousemove', onMouseOver, false);
canvas.addEventListener('mouseout', onMouseOut, false);
document.getElementById('resign-button').addEventListener('click', resign);



document.getElementById('walk-button').addEventListener('click', (e)=>{
    current_move_action = "walk";
    radioButtons();
});

document.getElementById('bomb-button').addEventListener('click', (e)=>{
    current_move_action = "bomb";
    radioButtons();
});