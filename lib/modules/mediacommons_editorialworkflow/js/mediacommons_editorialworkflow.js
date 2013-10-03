/**
 * Implementation of Drupal behavior.
 */

(function($) {
  
    Drupal.behaviors.mediacommons_editorialworkflow = {};
  
    Drupal.behaviors.mediacommons_editorialworkflow.attach = function(context) {
        if (window.console) console.log('attach mediacommons_editorialworkflow');
        
        /** Hide messages */
        
        /**
        setTimeout(function() {
          $('#console').hide();
        }, 3000);
        */
       
        $(".field-name-field-attached-images .image-preview img").not('.mediacommons_editorialworkflow_proceeded').each(function() {
            var currentTarget = $(this),
                targetArea = currentTarget.parent(),
                originalTarget = targetArea.next().children('span').children('a'),
                currentTargetSource = currentTarget.attr('src'),
                originalTargetSource = originalTarget.attr('href');
                currentTarget.addClass('mediacommons_editorialworkflow_proceeded');               
                targetArea.append('<div class="image-preview-actions"><a href="#crop" class="crop">crop</a> <a href="#remove" class="remove mdelete">remove</a></div>');               
                currentTarget.removeAttr('width');
                currentTarget.removeAttr('height');
                currentTarget.attr({ 'data-fullsize': originalTargetSource, 'data-preview': currentTargetSource });
        });
       
        $("body").delegate("#edit-field-type input:radio", "change", function(e) {
            var type = parseInt($(this).val(), 10);
            $('.field-name-field-video-embed-link, .field-name-field-representative-image, .field-name-field-tease-image').toggle();
        });
        
        $(".column-side").delegate(".image-preview img", "mouseenter", function(e) {
            var currentTarget = $(this);
            currentTarget.attr('src', currentTarget.attr('data-fullsize'));
        });
        
        $(".column-side").delegate(".image-preview img", "mouseleave", function(e) {
            currentTarget = $(this);
            currentTarget.attr('src', currentTarget.attr('data-preview'));
        });

  };  
})(jQuery);