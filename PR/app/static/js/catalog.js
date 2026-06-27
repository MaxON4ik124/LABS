const dialog = document.getElementById("purchase-dialog");
const purchaseForm = document.getElementById("purchase-form");
const quantityInput = document.getElementById("purchase-quantity");
const productNameElement = document.getElementById("purchase-product-name");
const productPriceElement = document.getElementById("purchase-product-price");
const totalElement = document.getElementById("purchase-total");
const responseSection = document.getElementById("xml-response-section");
const responseElement = document.getElementById("xml-response");

let selectedProduct = null;

function escapeXml(value) {
    return String(value)
        .replaceAll("&", "&amp;")
        .replaceAll("<", "&lt;")
        .replaceAll(">", "&gt;")
        .replaceAll('"', "&quot;")
        .replaceAll("'", "&apos;");
}

function updateTotal() {
    if (!selectedProduct) {
        return;
    }

    const quantity = Math.max(1, Number(quantityInput.value) || 1);
    totalElement.textContent = selectedProduct.price * quantity;
}

document
    .querySelectorAll(".purchase-button")
    .forEach(button => {
        button.addEventListener("click", () => {
            selectedProduct = {
                id: button.dataset.productId,
                name: button.dataset.productName,
                price: Number(button.dataset.productPrice)
            };

            quantityInput.value = "1";
            productNameElement.textContent = selectedProduct.name;
            productPriceElement.textContent = selectedProduct.price;
            updateTotal();
            dialog.showModal();
        });
    });

quantityInput.addEventListener("input", updateTotal);

document
    .getElementById("cancel-purchase-button")
    .addEventListener("click", () => {
        dialog.close();
    });

purchaseForm.addEventListener("submit", async event => {
    event.preventDefault();

    if (!selectedProduct) {
        return;
    }

    const quantity = Number(quantityInput.value);

    /*
     * Пользователь работает с обычным диалогом.
     * XML создаётся автоматически и отправляется Flask-серверу.
     */
    const xml = `<?xml version="1.0" encoding="UTF-8"?>
<order>
    <product_id>${escapeXml(selectedProduct.id)}</product_id>
    <quantity>${escapeXml(quantity)}</quantity>
</order>`;

    const submitButton =
        purchaseForm.querySelector("button[type='submit']");
    submitButton.disabled = true;

    try {
        const response = await fetch("/api/purchase", {
            method: "POST",
            headers: {
                "Content-Type": "application/xml"
            },
            body: xml
        });

        const result = await parseJsonResponse(response);

        const headerBalance = document.getElementById("header-balance");
        headerBalance.textContent = result.balance;

        /*
         * Результат обработки XML выводится только как текст.
         * innerHTML здесь не используется.
         */
        responseElement.textContent = JSON.stringify(
            result.xml_result,
            null,
            2
        );

        responseSection.classList.remove("hidden");
        responseSection.scrollIntoView({
            behavior: "smooth",
            block: "start"
        });

        dialog.close();
        showToast(result.message);
    } catch (error) {
        showToast(error.message, "error");
    } finally {
        submitButton.disabled = false;
    }
});
