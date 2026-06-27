document
    .getElementById("login-form")
    .addEventListener("submit", async event => {
        event.preventDefault();

        const button = event.target.querySelector("button[type='submit']");
        button.disabled = true;

        try {
            const response = await fetch("/api/login", {
                method: "POST",
                headers: {
                    "Content-Type": "application/json"
                },
                body: JSON.stringify({
                    username: document.getElementById("username").value,
                    password: document.getElementById("password").value
                })
            });

            const result = await parseJsonResponse(response);
            window.location.href = result.redirect;
        } catch (error) {
            showToast(error.message, "error");
        } finally {
            button.disabled = false;
        }
    });
