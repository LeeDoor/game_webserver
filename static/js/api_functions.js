
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

function resign(){
    move({ move_type: "resign" });
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
        if(json.events.filter(event => event.event_type == "player_won").length > 0){
            window.location.href = 'http://' + IPADDR + '/session_state.html?sessionId='+sessionId;
        }
        else{
            waitForStateChange();
            updateScene();
        }
    });
}