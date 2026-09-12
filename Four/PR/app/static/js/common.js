function showMessage(text, error=false) {
    const box = document.getElementById("message");
    box.textContent = text;
    box.className = error ? "error" : "success";
    setTimeout(() => box.className = "", 3000);
}

async function json(response) {
    const data = await response.json();
    if (!response.ok) throw new Error(data.error || "Ошибка");
    return data;
}

function escapeXml(value) {
    return String(value)
        .replaceAll("&", "&amp;")
        .replaceAll("<", "&lt;")
        .replaceAll(">", "&gt;")
        .replaceAll('"', "&quot;")
        .replaceAll("'", "&apos;");
}
