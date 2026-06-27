let toastTimer = null;

function showToast(message, type = "success") {
    const toast = document.getElementById("toast");

    if (!toast) {
        return;
    }

    clearTimeout(toastTimer);

    toast.textContent = message;
    toast.className = `toast ${type}`;

    toastTimer = setTimeout(() => {
        toast.classList.add("hidden");
    }, 3500);
}

async function parseJsonResponse(response) {
    let data;

    try {
        data = await response.json();
    } catch {
        throw new Error("Сервер вернул некорректный ответ");
    }

    if (!response.ok || data.success === false) {
        throw new Error(data.error || "Ошибка запроса");
    }

    return data;
}
