document
    .getElementById("change-password-form")
    .addEventListener("submit", async event => {
        event.preventDefault();

        const currentPassword =
            document.getElementById("current-password").value;

        const newPassword =
            document.getElementById("new-password").value;

        const newPasswordConfirm =
            document.getElementById("new-password-confirm").value;

        if (newPassword !== newPasswordConfirm) {
            showToast("Новые пароли не совпадают", "error");
            return;
        }

        const button = event.target.querySelector("button[type='submit']");
        button.disabled = true;

        try {
            const response = await fetch("/api/change-password", {
                method: "POST",
                headers: {
                    "Content-Type": "application/json"
                },
                body: JSON.stringify({
                    currentPassword,
                    newPassword,
                    newPasswordConfirm
                })
            });

            const result = await parseJsonResponse(response);
            event.target.reset();
            showToast(result.message);
        } catch (error) {
            showToast(error.message, "error");
        } finally {
            button.disabled = false;
        }
    });
