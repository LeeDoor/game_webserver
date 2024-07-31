
function showCustomPopup(text, color) {
    var existingPopups = document.getElementsByClassName('popup');
    var newPopup = document.createElement('div');
    newPopup.textContent = text;
    newPopup.style.color = color;
    newPopup.classList.add('popup');

    if (existingPopups.length > 0) {
        var lastPopup = existingPopups[existingPopups.length - 1];
        var lastPopupBottom = lastPopup.offsetTop + lastPopup.offsetHeight;
        if (lastPopupBottom > '500'){
            lastPopupBottom = '20'
        }
        newPopup.style.top = lastPopupBottom + 'px';
    } else  {
        newPopup.style.top = '20px'; 
    }
    newPopup.style.left = '20px'; 

    document.body.appendChild(newPopup);

    setTimeout(function() {
        newPopup.remove();
    }, 3000); 
}

function verifyToken(){
    let res = false; // true if login is valid
    fetch('http://localhost:9999/api/profile', {
        method: 'GET',
        headers: {
            'Content-Type': 'application/json',
            'Authorization':'Bearer ' + localStorage.getItem('token')
        }
    }).then(response=>{
        if (!response.ok){
            res = false;
            throw new Error('token expired');
        }
        else{
            res = true;
        }
    }).catch(reason=>res = false);
    if(res) return true;
    const data = {
        login: localStorage.getItem('login'),
        password: localStorage.getItem('password')
    };
    fetch('http://localhost:9999/api/login', {
        method: 'GET',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(data)
    }).then(response=>{
        if (!response.ok){
            res = false;
            throw new Error('login data wrong');
        }
        else{
            res = true;
            return response.json();
        }
    }).then(json=>localStorage.setItem('token', json.token)
    ).catch(reason=>res = false);
    return res;

}   

function WaitForOpponent(){
    fetch('http://localhost:9999/api/game/wait_for_opponent', {
        method: 'GET',
        headers: {
            'Content-Type': 'application/json',
            'Authorization':'Bearer ' + localStorage.getItem('token')
        },
    }).then(async response=>{
        if(response.ok){
            showCustomPopup("game found!", "#00FF00");
            const data = await response.json();
            window.location.href = "http://localhost:9999/field.html?sessionId="+data.sessionId;
        }
    });
}
function Enqueue(event) {
    fetch('http://localhost:9999/api/game/enqueue', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Authorization':'Bearer ' + localStorage.getItem('token')
        },
    }).then(response=>{
        if(response.ok){
            showCustomPopup("queuing...", "#00FF00");
            WaitForOpponent();
        }
        return response.json();
    }).then(function (json){
        if(json.error_name){
            showCustomPopup("failed to add you to the queue: " + json.description, "#FF0000");
        }
    });
}
function Register(event) {
    const regLogin = document.getElementById('regLogin').value;
    const regPass = document.getElementById('regPass').value;

    const data = {
        login: regLogin,
        password: regPass 
    };

    fetch('http://localhost:9999/api/register', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(data)
    });
}
function Login(event) {
    const logLogin = document.getElementById('logLogin').value;
    const logPass = document.getElementById('logPass').value;

    const data = {
        login: logLogin,
        password: logPass 
    };

    fetch('http://localhost:9999/api/login', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(data)
    }).then(response => {
        if (response.ok) {
            showCustomPopup("login successful as " + data.login, "#00FF00"); 
            document.getElementById('loginText').textContent = data.login;
            localStorage.setItem('login', data.login);
            localStorage.setItem('password', data.password);
            return response.json();
        } else {
            showCustomPopup("login failed. check your data and try again", "#FF0000"); 
            throw new Error('login failed');
        }
    }).then(json=>{
        console.log(json.token);
        localStorage.setItem('token', json.token);
    });
}
function DOMCL() {
    var lt = document.getElementById('loginText');
    if (lt){
        lt.textContent = localStorage.getItem('login');
        if (lt.textContent == ''){
            lt.textContent = 'Guest';
        }
    }
}
document.getElementById('registerButton').addEventListener('click', Register);
document.getElementById('logButton').addEventListener('click', Login);
document.getElementById('playButton').addEventListener('click', Enqueue);
document.addEventListener("DOMContentLoaded", DOMCL);