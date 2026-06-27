document
    .getElementById("register-form")
    .addEventListener("submit", async event => {
        event.preventDefault();

        const password = document.getElementById("password").value;
        const passwordConfirm =
            document.getElementById("password-confirm").value;

        if (password !== passwordConfirm) {
            showToast("Пароли не совпадают", "error");
            return;
        }

        const button = event.target.querySelector("button[type='submit']");
        button.disabled = true;

        try {
            const response = await fetch("/api/register", {
                method: "POST",
                headers: {
                    "Content-Type": "application/json"
                },
                body: JSON.stringify({
                    username: document.getElementById("username").value,
                    password,
                    passwordConfirm
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
