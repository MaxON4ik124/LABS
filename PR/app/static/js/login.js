document.getElementById("login-form").addEventListener("submit", async event => {
    event.preventDefault();
    try {
        const data = await json(await fetch("/api/login", {
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
