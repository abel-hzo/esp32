let button = document.querySelector(".button");
let active = document.getElementById("active");

active.disabled = true;

let socketConnection = new WebSocket('ws://' + location.hostname + ':81/');

fetch("http://" + location.hostname + "/status")
.then(response => response.json())
.then(data => {
    button.textContent = data.accion;
    active.checked = data.estado;
});

socketConnection.onopen = function (event) {
    active.disabled = false;
}

socketConnection.onmessage = function (event) {

    console.log(event.data);
    var data = JSON.parse(event.data);

    button.textContent = data.accion;
    active.checked = data.estado;
}

active.addEventListener("change", (e) => {
    if (active.checked) {
        socketConnection.send("on");
    } else {
        socketConnection.send("off");
    }
});