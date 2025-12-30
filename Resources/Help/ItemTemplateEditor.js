(function () {
	document.addEventListener("click", function (event) {
		const link = event.target.closest("a[data-type]");

		if (!link) return;

		event.preventDefault();

		const url = link.getAttribute("href") || "";
		const dataType = link.getAttribute("data-type") || "";

		console.log("MIAE_LINK:" + dataType + ":" + url);
	});

	var toggles = document.querySelectorAll("[data-toggle]");
	toggles.forEach(function (toggle) {
		toggle.addEventListener("click", function () {
			var endpoint = toggle.closest("[data-endpoint]");
			var body = endpoint.querySelector(".endpoint-body");
			var isOpen = body.classList.contains("open");
			body.classList.toggle("open", !isOpen);
			toggle.textContent = isOpen ? "Show details" : "Hide details";
		});
	});
})();
