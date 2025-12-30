(function () {
	document.addEventListener("click", function (event) {
		const link = event.target.closest("a[data-type]");

		if (!link) return;

		event.preventDefault();

		const url = link.getAttribute("href") || "";
		const dataType = link.getAttribute("data-type") || "";

		console.log("MIAE_LINK:" + dataType + ":" + url);
	});

	document.addEventListener("contextmenu", function (event) {
		event.preventDefault();
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

	document.addEventListener('keydown', function(event) {
		if (event.altKey && (event.key === 'ArrowLeft' || event.key === 'ArrowRight')) {
			event.preventDefault();
			event.stopPropagation();
			return false;
		}
		
		if (event.key === 'Backspace' && 
			!['INPUT', 'TEXTAREA'].includes(event.target.tagName)) {
			event.preventDefault();
			event.stopPropagation();
			return false;
		}
	}, true);

	history.pushState(null, null, location.href);
	
	window.addEventListener('popstate', function(event) {
		history.pushState(null, null, location.href);
	});	
})();
