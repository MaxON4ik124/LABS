document.getElementById("register-form").addEventListener("submit", async event => {
    event.preventDefault();
    try {
        const data = await json(await fetch("/api/register", {
            method: "POST",
            headers: {"Content-Type": "application/json"},
            body: JSON.stringify({
                username: username.value,
                password: password.value
            })
        }));
        location.href = data.redirect;
    } catch (error) {
        showMessage(error.message, true);
    }
});
