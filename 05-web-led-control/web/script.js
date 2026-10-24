// ==========================================
// JAVASCRIPT KONTROL LED ESP8266
// ==========================================

const ipText = document.getElementById("ip");
const btnConnect = document.getElementById("btnConnect");
const btnOn = document.getElementById("btnOn");
const btnOff = document.getElementById("btnOff");
const statusText = document.getElementById("status");

// IP ESP8266 dari Serial Monitor ("Alamat IP: ...")
// Ganti di sini jika IP ESP berubah
const ESP_IP = "172.16.12.148";

// Batas waktu menunggu balasan ESP (milidetik)
const TIMEOUT = 5000;

// Menampilkan IP ESP di halaman
ipText.textContent = ESP_IP;

// ==========================================
// MENGIRIM PERINTAH KE ESP8266
// path: "/on", "/off", atau "/status"
// ==========================================

async function kirim(path) {
  const base = "http://" + ESP_IP;

  statusText.textContent = "Menghubungi " + base + path + " ...";

  const controller = new AbortController();
  const timer = setTimeout(function () {
    controller.abort();
  }, TIMEOUT);

  try {
    const res = await fetch(base + path, { signal: controller.signal });

    if (!res.ok) {
      throw new Error("HTTP " + res.status);
    }

    const data = await res.json();

    tampilkanStatus(data.led);
  } catch (err) {
    if (err.name === "AbortError") {
      statusText.textContent =
        "ESP8266 tidak menjawab. Cek IP dan pastikan satu Wi-Fi.";
    } else {
      statusText.textContent =
        "Gagal terhubung. Cek IP, Wi-Fi, dan pastikan sketch terbaru sudah di-upload.";
    }
    console.error(err);
  } finally {
    clearTimeout(timer);
  }
}

// ==========================================
// MENAMPILKAN STATUS LED
// ==========================================

function tampilkanStatus(led) {
  if (led === "on") {
    statusText.textContent = "Terhubung - Status LED: NYALA";
  } else {
    statusText.textContent = "Terhubung - Status LED: MATI";
  }
}

// ==========================================
// EVENT TOMBOL
// ==========================================

btnConnect.addEventListener("click", function () {
  kirim("/status");
});

btnOn.addEventListener("click", function () {
  kirim("/on");
});

btnOff.addEventListener("click", function () {
  kirim("/off");
});

// Langsung cek koneksi ke ESP saat halaman dibuka
kirim("/status");
