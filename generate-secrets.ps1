# ==========================================
# MEMBUAT secrets.h DARI .env
# Arduino tidak bisa membaca .env langsung,
# jadi isi .env disalin menjadi secrets.h di setiap
# folder sketch yang memakai #include "secrets.h"
#
# Cara pakai:
#   powershell -ExecutionPolicy Bypass -File .\generate-secrets.ps1
# ==========================================

$root = $PSScriptRoot
$envFile = Join-Path $root ".env"

if (-not (Test-Path $envFile)) {
    Write-Host "File .env tidak ditemukan. Salin .env.example menjadi .env lalu isi SSID & password Wi-Fi."
    exit 1
}

# Membaca isi .env (format KEY=VALUE)
$vars = [ordered]@{}

foreach ($line in Get-Content $envFile -Encoding UTF8) {
    $line = $line.Trim()

    if ($line -eq "" -or $line.StartsWith("#") -or -not $line.Contains("=")) {
        continue
    }

    $key, $value = $line -split "=", 2
    $value = $value.Trim()

    # Buang tanda kutip jika nilai ditulis "seperti ini"
    if ($value.Length -ge 2 -and $value.StartsWith('"') -and $value.EndsWith('"')) {
        $value = $value.Substring(1, $value.Length - 2)
    }

    $vars[$key.Trim()] = $value
}

# Menyusun isi secrets.h
$lines = @(
    "// ==========================================",
    "// KREDENSIAL WI-FI (JANGAN DI-COMMIT)",
    "// Dibuat otomatis dari .env oleh generate-secrets.ps1",
    "// Ubah .env lalu jalankan ulang script, jangan edit file ini",
    "// ==========================================",
    ""
)

foreach ($key in $vars.Keys) {
    # Escape \ dan " agar aman di dalam string C
    $escaped = $vars[$key].Replace('\', '\\').Replace('"', '\"')
    $lines += "#define $key `"$escaped`""
}

# Menulis secrets.h ke setiap folder sketch yang membutuhkannya
$sketches = Get-ChildItem -Path $root -Filter *.ino -Recurse |
    Where-Object { $_.FullName -notlike "*\libraries\*" } |
    Where-Object { Select-String -Path $_.FullName -Pattern '#include "secrets.h"' -SimpleMatch -Quiet }

foreach ($sketch in $sketches) {
    $out = Join-Path $sketch.DirectoryName "secrets.h"
    [System.IO.File]::WriteAllLines($out, [string[]]$lines)
    Write-Host "secrets.h dibuat: $out"
}
