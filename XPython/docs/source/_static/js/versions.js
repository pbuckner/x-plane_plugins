jQuery(function() {
    jQuery('.version-selector').on('change', function() {
        var files = location.pathname.split('/');
        location.pathname = [files[0], files[1], jQuery(this).val()].concat(files.slice(3)).join('/');
    });
});
