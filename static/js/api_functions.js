
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
    });
}

function walk(){
    const data = {
        move_type: "walk",
        position: selectedCell.position
    };
    if(!now_turn)
        return false;
    if(Math.abs(data.position.x - playerUs.position.x) + Math.abs(data.position.y - playerUs.position.y) != 1)
        return false;

    move(data);
    return true;
}

function placeBomb(){
    const data = {
        move_type: "place_bomb",
        position: selectedCell.position
    };
    if(!now_turn)
        return false;
    if( Math.abs(data.position.x - playerUs.position.x) > 1 &&
        Math.abs(data.position.y - playerUs.position.y) > 1)
        return false;

    move(data);
    return true;
}

function placeGun(){
    let data = {
        move_type: "place_gun",
        position: selectedCell.position,
    };

    if(selectedCell.position.x > selectedDirectionCell.position.x){
        data.direction = "left";
    }
    else if(selectedCell.position.x < selectedDirectionCell.position.x) {
        data.direction = "right";
    }
    else if(selectedCell.position.y < selectedDirectionCell.position.y) {
        data.direction = "down";
    }
    else {
        data.direction = "up";
    }
    

    if(!now_turn)
        return false;
    if( Math.abs(data.position.x - playerUs.position.x) > 1 &&
        Math.abs(data.position.y - playerUs.position.y) > 1)
        return false;

    move(data);
    return true;
}


function resign(){
    move({ move_type: "resign" });
}

// asyncronically waits for opponent move
function waitForStateChange() {
    fetch('http://' + IPADDR + '/api/game/session_state_change?sessionId='+sessionId+"&from_move="+last_move, 
    {
        method: 'GET',
        headers: {
            'Content-Type': 'application/json',
            'Authorization':'Bearer ' + localStorage.getItem('token')
        },
    }).then(response=>{
        if (!response.ok)
            window.location.href = 'http://' + IPADDR + '/session_state.html?sessionId='+sessionId;
        return response.json();
    }).then(async json=>{
        await handleEvents(json);
        waitForStateChange();
    });
}