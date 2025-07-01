let button = document.querySelector(".button");
let active = document.getElementById("active");

active.addEventListener("change", (e) => {
    if(active.checked) {
        fetching("on");
    } else {
        fetching("off");
    }
});

function fetching(action) {
    fetch("http://"+location.hostname+"/"+action)
    .then(response => response.json())
    .then(data => {
        //console.log(data);
        button.textContent = data.accion;
        active.checked = data.estado;
    });
}

document.body.onload = function() {
    fetching("status");
}