/**
 * Implementation of Drupal behavior.
 */
(function($) {

    Drupal.behaviors.mediacommons_ui = {};
  
    Drupal.behaviors.mediacommons_ui.attach = function(context) {
        if (window.console) console.log('mediacommons_ui');
        
        $("body").delegate("#edit-field-type input:radio", "change", function(e) {
            var type = parseInt($(this).val(), 10);
            $('.field-name-field-video-embed-link, .field-name-field-representative-image, .field-name-field-tease-image').toggle();
        });
  };  
})(jQuery);