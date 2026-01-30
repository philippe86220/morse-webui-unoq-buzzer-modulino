const speed = document.getElementById("speed");
const speedVal = document.getElementById("speedVal");

speed.addEventListener("input", () => {
  speedVal.textContent = speed.value;
});

async function getJSON(url) {
  const r = await fetch(url.toString());
  return await r.json();
}

const MORSE = {
  "A": ".-",
  "B": "-...",
  "C": "-.-.",
  "D": "-..",
  "E": ".",
  "F": "..-.",
  "G": "--.",
  "H": "....",
  "I": "..",
  "J": ".---",
  "K": "-.-",
  "L": ".-..",
  "M": "--",
  "N": "-.",
  "O": "---",
  "P": ".--.",
  "Q": "--.-",
  "R": ".-.",
  "S": "...",
  "T": "-",
  "U": "..-",
  "V": "...-",
  "W": ".--",
  "X": "-..-",
  "Y": "-.--",
  "Z": "--..",
  "0": "-----",
  "1": ".----",
  "2": "..---",
  "3": "...--",
  "4": "....-",
  "5": ".....",
  "6": "-....",
  "7": "--...",
  "8": "---..",
  "9": "----.",
  ".": ".-.-.-",
  ",": "--..--",
  "?": "..--..",
  "/": "-..-.",
  "-": "-....-",
  ":": "---...",
  ";": "-.-.-.",
  "@": ".--.-."
};

const out = document.getElementById("out");
const txt = document.getElementById("txt");
const morseView = document.getElementById("morseView");

function escapeHtml(s) {
  return s
    .replaceAll("&", "&amp;")
    .replaceAll("<", "&lt;")
    .replaceAll(">", "&gt;");
}

function renderSymbols(morse) {
  // morse string like ".-.-"
  // returns HTML with dot/dash blocks
  let html = '<span class="morseLine">';
  for (const c of morse) {
    if (c === ".") html += '<span class="dot"></span>';
    else if (c === "-") html += '<span class="dash"></span>';
    else html += '<span class="gap"></span>';
  }
  html += "</span>";
  return html;
}

function buildRepresentation(text) {
  // Show each character with its morse and the graphical blocks
  const rows = [];
  for (const ch of text) {
    if (ch === " ") {
      rows.push('<div class="row"><span class="small">(espace)</span><span class="word"></span></div>');
      continue;
    }

    const up = ch.toUpperCase();
    const morse = MORSE[up] || "";
    if (!morse) {
      rows.push(
        '<div class="row"><b>' + escapeHtml(ch) + '</b> : <span class="small">(non supporte)</span></div>'
      );
      continue;
    }

    rows.push(
      '<div class="row"><b>' + escapeHtml(ch) + '</b> : ' +
      
      renderSymbols(morse) +
      "</div>"
    );
  }
  return rows.join("");
}

function refreshMorseView() {
  morseView.innerHTML = buildRepresentation(txt.value);
}

document.getElementById("status").onclick = async () => {
  try {
    out.textContent = "Status...";
    const url = new URL("/status", window.location.href);
    const res = await getJSON(url);
    out.textContent = JSON.stringify(res, null, 2);
  } catch (e) {
    out.textContent = "Erreur: " + e;
  }
};

document.getElementById("send").onclick = async () => {
 if (!txt.value.trim()) {
  // Message d'information (zone API masquée pour l'instant)
     out.textContent = "Texte vide";
  return;
}
  try {
    out.textContent = "Envoi...";
    const url = new URL("/morse", window.location.href);
    url.searchParams.set("data", txt.value);
    url.searchParams.set("speed", speed.value);

    const res = await getJSON(url);
    out.textContent = JSON.stringify(res, null, 2);
  } catch (e) {
    out.textContent = "Erreur: " + e;
  }
};


document.getElementById("clear").onclick = (e) => {
  e.preventDefault();
  txt.value = "";
  out.textContent = "";
  refreshMorseView();
  txt.focus();
};

txt.addEventListener("input", refreshMorseView);

// Init
refreshMorseView();

