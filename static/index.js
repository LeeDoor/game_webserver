IPADDR = '95.220.184.224:8080';
const popups = document.getElementById("popups");

function showCustomPopup(text, color) {
    var existingPopups = document.getElementsByClassName('popup');
    var newPopup = document.createElement('div');
    newPopup.textContent = text;
    newPopup.style.color = color;
    newPopup.classList.add('popup');

    if (existingPopups.length > 0 && existingPopups.length < 10) {
        var lastPopup = existingPopups[existingPopups.length - 1];
        var lastPopupBottom = lastPopup.offsetTop + lastPopup.offsetHeight;
        newPopup.style.top = lastPopupBottom + 'px';
    } else  {
        popups.innerHTML = '';
        newPopup.style.top = '20px'; 
    }
    newPopup.style.left = '20px'; 

    popups.appendChild(newPopup);

    setTimeout(function() {
        newPopup.remove();
    }, 10000); 
}

function verifyToken(){
    let res = false; // true if login is valid
    fetch('http://' + IPADDR + '/api/profile', {
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
    fetch('http://' + IPADDR + '/api/login', {
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

function MatchFound(sessionId){
    showCustomPopup("game found!", "#00FF00");
    const href = 'http://' + IPADDR + '/field.html?sessionId='+sessionId;
    window.location.href = href;
}

function WaitForOpponent(){
    fetch('http://' + IPADDR + '/api/game/wait_for_opponent', {
        method: 'GET',
        headers: {
            'Content-Type': 'application/json',
            'Authorization':'Bearer ' + localStorage.getItem('token')
        },
    }).then(async response=>{
        if(response.ok){
            const data = await response.json();
            MatchFound(data.sessionId);
        }
    });
}
function Enqueue(event) {
    fetch('http://' + IPADDR + '/api/game/enqueue', {
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
        if(json.sessionId){
            return MatchFound(json.sessionId);
        }
        if(json.description){
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

    if (data.login.length < 3){
        showCustomPopup("make sure your login is more than 2 symbols", "#FF0000");
        return;
    }
    if (data.password.length < 6){
        showCustomPopup("make sure your password is more than 5 symbols", "#FF0000");
        return;
    }
    if (!/\d/.test(data.password)){
        showCustomPopup("make sure your password has a digit", "#FF0000");
        return;
    }
        

    fetch('http://' + IPADDR + '/api/register', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(data)
    }).then(response=>{
        if(response.ok){
            document.getElementById('logLogin').value = data.login;
            document.getElementById('logPass').value = data.password;
            showCustomPopup("success! welcome, " + data.login, "#00FF00");
            Login(event);
        }
        else
            showCustomPopup("login is taken. please come up with a new username or add a couple of numbers to your nickname like a fucker.", "#00FF00");
    });
}
function Login(event) {
    const logLogin = document.getElementById('logLogin').value;
    const logPass = document.getElementById('logPass').value;

    const data = {
        login: logLogin,
        password: logPass 
    };

    fetch('http://' + IPADDR + '/api/login', {
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
            document.getElementById('playButton').disabled = false;
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
    if(localStorage.getItem('token'))
        showCustomPopup("Hi, " +  localStorage.getItem('login') + "! click play!!!", "#00FF00");
    else
        document.getElementById('playButton').disabled = true;
}
document.getElementById('registerButton').addEventListener('click', Register);
document.getElementById('logButton').addEventListener('click', Login);
document.getElementById('playButton').addEventListener('click', Enqueue);
document.addEventListener("DOMContentLoaded", DOMCL);