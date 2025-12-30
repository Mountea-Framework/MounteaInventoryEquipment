(function() {
    document.addEventListener('click', function(event) {
        const link = event.target.closest('a[data-type]');
        
        if (!link) return;
        
        event.preventDefault();
        
        const url = link.getAttribute('href') || '';
        const dataType = link.getAttribute('data-type') || '';
        
        if (window.ueHandler && window.ueHandler.HandleLinkClick) {
            window.ueHandler.HandleLinkClick(url, dataType);
        }
    });
})();