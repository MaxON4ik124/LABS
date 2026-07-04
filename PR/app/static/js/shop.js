const dialog = document.getElementById("dialog");
let selected;

document.querySelectorAll(".buy").forEach(button => {
    button.onclick = () => {
        selected = button.dataset;
        document.getElementById("selected-name").textContent = selected.name;
        dialog.showModal();
    };
});

document.getElementById("cancel").onclick = () => dialog.close();

document.getElementById("purchase-form").addEventListener("submit", async event => {
    event.preventDefault();

    const payload = `<?xml version="1.0" encoding="UTF-8"?>
<order>
    <product_id>${escapeXml(selected.id)}</product_id>
    <quantity>${escapeXml(quantity.value)}</quantity>
    <comment>${escapeXml(comment.value)}</comment>
</order>`;

    try {
        const data = await json(await fetch("/api/purchase", {
            method: "POST",
            headers: {"Content-Type": "application/xml"},
            body: payload
        }));

        document.getElementById("balance").textContent = data.balance;
        dialog.close();
        showMessage(data.message);
    } catch (error) {
        showMessage(error.message, true);
    }
});
