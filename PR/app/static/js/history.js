document
    .querySelectorAll(".order-date")
    .forEach(element => {
        const date = new Date(element.dataset.date);

        if (!Number.isNaN(date.getTime())) {
            element.textContent = new Intl.DateTimeFormat("ru-RU", {
                dateStyle: "medium",
                timeStyle: "short"
            }).format(date);
        }
    });
