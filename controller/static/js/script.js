const serverIP = document.body.dataset.serverIp;
const serverUrl = `http://${serverIP}:3030/`;

function sendCommand(id, action) {
  fetch(serverUrl, {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({
      id: id,
      action: action
    })
  }).then(res => res.json())
    .then(data => console.log("Risposta:", data))
    .catch(err => alert("Errore:", err));
}

// Funzione per iniziare invio continuo
function startCommand(id) {
  sendCommand(id, "press");
}

// Funzione per fermare invio continuo
function stopCommand(id) {
  sendCommand(id, "release");
}

// Aggiunge listener a ogni bottone
document.querySelectorAll("button").forEach(button => {
  // Eventi mouse
  button.addEventListener("mousedown", () => startCommand(button.id));
  button.addEventListener("mouseup", () => stopCommand(button.id));
  button.addEventListener("mouseleave", () => stopCommand(button.id));

  // Eventi touch (mobile)
  button.addEventListener("touchstart", (e) => {
    e.preventDefault(); // evita doppio evento
    startCommand(button.id);
  }, { passive: false });

  button.addEventListener("touchend", () => stopCommand(button.id));
});