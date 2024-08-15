document.addEventListener("DOMContentLoaded", function() {
    const urlParams = new URLSearchParams(window.location.search);
    const sessionId = urlParams.get('sessionId');
    const token = localStorage.getItem('token');
    const login = localStorage.getItem('login');

    if (sessionId && token && login) {
        fetch('http://' + IPADDR + '/api/game/session_state?sessionId=' + sessionId, {
            method: 'GET',
            headers: {
                'Authorization': `Bearer ${token}`
            }
        })
        .then(response => response.json())
        .then(data => {
            const resultDiv = document.getElementById('result');
            if (data.winner === login) {
                resultDiv.textContent = "You Won!";
                resultDiv.classList.add('win');
            } else {
                resultDiv.textContent = "You Lost!";
                resultDiv.classList.add('lose');
            }
        })
        .catch(error => console.error('Error fetching session state:', error));
    } else {
        console.error('Required data is missing from local storage or URL parameters.');
    }

    document.getElementById('redirectButton').addEventListener('click', function() {
        window.location.href = 'http://' + IPADDR + '/index.html';
    });
});